"""Reusable live peripheral view: user controls -> C++ process -> observations."""
from __future__ import annotations

import atexit
import asyncio
from dataclasses import dataclass
from html import escape
from queue import Queue, Empty
import re
import subprocess
import threading

import ipywidgets as widgets
from IPython.display import display
from notebook_theme import SUCCESS, FAILURE, html, panel, controls, check_card

PANELS = {}


@dataclass(frozen=True)
class Observation:
    time_ms: int
    pressed: bool
    led: bool


class Session:
    """Persistent Controller process, with bounded reads on all host platforms."""
    def __init__(self, lab):
        self.process = subprocess.Popen(
            [str(lab.executable('simulator')), '--protocol'],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, bufsize=1,
        )
        self.queue = Queue()
        self.reader = threading.Thread(target=self._read, daemon=True)
        self.reader.start()
        self.initial = self._receive()

    def _read(self):
        for line in self.process.stdout:
            self.queue.put(line)
        self.queue.put(None)

    def _receive(self):
        try:
            line = self.queue.get(timeout=3)
        except Empty:
            self.close()
            raise RuntimeError('C++ 在 3 秒内没有响应；检查 tick() 是否有阻塞或死循环。')
        match = re.fullmatch(r't=(\d+)ms button=(HIGH|LOW) led=(ON|OFF)\n?', line or '')
        if not match:
            self.close()
            raise RuntimeError(f'C++ 模拟进程异常：{line or "已退出"}')
        return Observation(int(match[1]), match[2] == 'LOW', match[3] == 'ON')

    def send(self, command):
        if command not in {'press', 'release', 'tick', 'reset', 'boot-held'}:
            raise ValueError('Invalid simulator command')
        self.process.stdin.write(command + '\n')
        self.process.stdin.flush()
        return self._receive()

    def close(self):
        if self.process.poll() is None:
            self.process.terminate()
            try:
                self.process.wait(timeout=1)
            except subprocess.TimeoutExpired:
                self.process.kill()
                self.process.wait()
        self.reader.join(timeout=1)
        for pipe in (self.process.stdin, self.process.stdout):
            if pipe and not pipe.closed:
                pipe.close()


class Contract:
    """Behaviour oracle, independent of the learner's Controller state."""
    def __init__(self, profile):
        self.profile = profile
        self.previous = None
        self.expected = False

    def observe(self, observation):
        if self.profile == 'guided':
            self.expected = observation.pressed
        elif self.profile == 'exercise':
            if self.previous is not None and observation.pressed and not self.previous:
                self.expected = not self.expected
        self.previous = observation.pressed
        return None if self.profile is None else self.expected


def board_svg(observation, mismatch=False):
    """A teaching schematic, not the physical pin order of every ESP32 board."""
    led = '#ffd24a' if observation.led else '#46515e'
    glow = '<circle cx="470" cy="77" r="31" fill="#ffda57" opacity=".25"/>' if observation.led else ''
    stroke = FAILURE if mismatch else SUCCESS
    button = '按下 / LOW' if observation.pressed else '松开 / HIGH'
    return f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 600 200" width="100%" font-family="system-ui, sans-serif" role="img" aria-label="ESP32 示意板，按钮{button}，LED {'ON' if observation.led else 'OFF'}">
    <rect x="2" y="2" width="596" height="196" rx="15" fill="#141414" stroke="{stroke}" stroke-width="3"/>
    <rect x="36" y="18" width="232" height="167" rx="10" fill="#153f47"/>
    <rect x="86" y="45" width="126" height="55" rx="4" fill="#adb7c0"/>
    <text x="149" y="78" text-anchor="middle" fill="#16252f" font-size="17">ESP32</text>
    <text x="54" y="128" fill="white" font-size="13">GPIO18 · LED 输出</text>
    <text x="54" y="155" fill="white" font-size="13">GPIO19 · Button 输入</text>
    <text x="54" y="182" fill="white" font-size="13">3V3 / GND</text>
    <rect x="248" y="118" width="20" height="8" fill="#d7b35e"/>
    <rect x="248" y="145" width="20" height="8" fill="#d7b35e"/>
    <path d="M268 122 H312 V77 H345" fill="none" stroke="{stroke}" stroke-width="3"/>
    <path d="M345 77 l6 -6 l8 12 l8 -12 l8 12 l8 -6 H448" fill="none" stroke="#b8c2cc" stroke-width="2"/>
    <text x="373" y="55" text-anchor="middle" fill="#eeeeee" font-size="14">330 Ω</text>
    {glow}<circle cx="470" cy="77" r="22" fill="{led}" stroke="#b8c2cc" stroke-width="3"/>
    <text x="516" y="82" fill="#eeeeee" font-size="17">{'● ON' if observation.led else '○ OFF'}</text>
    <path d="M470 100 V114 H518" fill="none" stroke="#b8c2cc" stroke-width="2"/>
    <text x="526" y="119" fill="#eeeeee" font-size="14">GND</text>
    <path d="M268 149 H373" fill="none" stroke="{stroke}" stroke-width="3"/>
    <rect x="376" y="134" width="123" height="45" rx="8" fill="{'#57431c' if observation.pressed else '#30343b'}" stroke="{'#e8bb55' if observation.pressed else '#798491'}" stroke-width="2"/>
    <text x="437" y="162" text-anchor="middle" fill="{'#ffe09a' if observation.pressed else '#eeeeee'}" font-size="15">{button}</text>
    </svg>'''


class PeripheralPanel:
    def __init__(self, lab, profile=None):
        self.lab, self.profile = lab, profile
        self.session = Session(lab)
        self.contract = Contract(profile)
        self.first_failure = None
        self.task = None
        self.clock_task = None
        self.closed = False
        self.syncing = False
        self.diagram = html()
        self.status = html()
        self.checks = html()
        self.button = widgets.ToggleButton(description='按下按钮', icon='hand-pointer-o',
                                           layout=widgets.Layout(width='150px', height='42px'))
        self.tick_button = widgets.Button(description='采样 +10 ms', icon='step-forward')
        self.clock = widgets.ToggleButton(description='连续采样（慢放）', icon='play',
                                          layout=widgets.Layout(width='185px'))
        self.reset_button = widgets.Button(description='复位', icon='refresh')
        self.check_button = widgets.Button(description='播放自动检查', icon='check', button_style='info')
        self.controls = [self.button, self.tick_button, self.clock, self.reset_button, self.check_button]
        for control, tip in zip(self.controls, [
            '切换外部按钮的按下 / 松开状态，并立即采样',
            '按钮状态不变，继续调用一次 C++ tick()',
            '每 250 ms 推进一次采样；再次点击暂停',
            '重置 C++ 控制器并清除首次失败记录',
            '逐步操作按钮，展示行为契约的检查结果',
        ]):
            control.tooltip = tip
        self.view = panel([
            html('<div class="academy-title">Virtual ESP32 · 实验台</div><p class="academy-note">LED 状态来自本格 C++ 的实际输出。图中引脚位置为教学示意。</p>'),
            self.diagram,
            html('<p class="academy-note">GPIO19 内部上拉至 3V3；按钮按下时连接 GND。</p>'),
            controls([self.button, self.tick_button, self.clock]),
            controls([self.reset_button, self.check_button]), self.status, self.checks,
        ])
        self.button.observe(self._button_changed, names='value')
        self.tick_button.on_click(lambda _: self.step('tick'))
        self.reset_button.on_click(lambda _: self.reset())
        self.check_button.on_click(self._start_checks)
        self.clock.observe(self._clock_changed, names='value')
        self.render(self.session.initial)

    def render(self, observation):
        expected = self.contract.observe(observation)
        mismatch = expected is not None and observation.led != expected
        if mismatch and self.first_failure is None:
            self.first_failure = (observation.time_ms, expected, observation.led)
        self.last = observation
        self.diagram.value = board_svg(observation, mismatch)
        self.button.description = '松开按钮' if observation.pressed else '按下按钮'
        self.button.button_style = 'warning' if observation.pressed else ''
        self.syncing = True
        self.button.value = observation.pressed
        self.syncing = False
        contract = {'guided': '按住亮，松开灭', 'exercise': '每次新按下只切换一次', None: '自由观察，不判对错'}[self.profile]
        if self.first_failure:
            time_ms, wanted, actual = self.first_failure
            message = f'✗ 首次不符：t={time_ms} ms，预期 LED {"ON" if wanted else "OFF"}，实际 {"ON" if actual else "OFF"}。复位可清除。'
            color = FAILURE
        else:
            message = '✓ 当前符合契约' if expected is not None else '自由观察'
            color = SUCCESS
        self.status.value = f'<b>t={observation.time_ms} ms</b> · {contract}<br><span style="color:{color};font-weight:bold">{escape(message)}</span>'
        return not mismatch

    def step(self, command):
        if self.closed:
            return False
        try:
            return self.render(self.session.send(command))
        except Exception as error:
            self.status.value = f'<b style="color:{FAILURE}">运行异常：{escape(str(error))}</b>'
            self.close()
            return False

    def reset(self, held=False):
        self.contract = Contract(self.profile)
        self.first_failure = None
        return self.step('boot-held' if held else 'reset')

    def _button_changed(self, change):
        if not self.syncing and not self.closed:
            self.step('press' if change['new'] else 'release')

    async def _clock_loop(self):
        try:
            while self.clock.value and not self.closed:
                await asyncio.sleep(.25)
                self.step('tick')
        except asyncio.CancelledError:
            pass

    def _clock_changed(self, change):
        if change['new']:
            self.clock_task = asyncio.create_task(self._clock_loop())
        elif self.clock_task:
            self.clock_task.cancel()

    def _start_checks(self, _):
        self.clock.value = False
        self.task = asyncio.create_task(self.animate_checks())

    def suite_html(self):
        result = self.lab.test_result(self.profile)
        if result.returncode not in (0, 1):
            raise RuntimeError(result.stderr or 'C++ 检查无法完成')
        cards = []
        for line in result.stdout.splitlines():
            if line.startswith(('[PASS]', '[FAIL]')):
                color = SUCCESS if line.startswith('[PASS]') else FAILURE
                cards.append(check_card(f'<span style="color:{color}">{escape(line)}</span>', color))
            elif line.startswith('       '):
                cards.append(f'<div style="color:{FAILURE};padding-left:12px">{escape(line.strip())}</div>')
        return '<b>完整 C++ 行为检查</b>' + ''.join(cards)

    def show_suite(self):
        self.checks.value = self.suite_html()

    async def animate_checks(self, delay=.25):
        if self.profile is None:
            self.checks.value = '自由观察格不判对错；请在 guided 或 exercise 格播放检查。'
            return
        scenarios = [
            ('上电 / 首次采样', ['reset']),
            ('按下按钮', ['reset', 'press']),
            ('保持按住，继续采样', ['reset', 'press'] + ['tick'] * 5),
            ('松开按钮', ['reset', 'press', 'release']),
            ('再次按下', ['reset', 'press', 'release', 'press', 'tick']),
            ('上电已按住', ['boot-held', 'tick', 'release', 'press']),
        ]
        results = []
        for control in self.controls:
            control.disabled = True
        try:
            for title, commands in scenarios:
                good = True
                failure = ''
                self.checks.value = ''.join(results) + f'<p>◌ 正在检查：{title}</p>'
                for command in commands:
                    passed = self.reset(command == 'boot-held') if command in {'reset', 'boot-held'} else self.step(command)
                    if not passed and not failure:
                        failure = self.status.value
                    good = good and passed
                    await asyncio.sleep(delay)
                color, mark = (SUCCESS, '✓') if good else (FAILURE, '✗')
                results.append(check_card(f'<b style="color:{color}">{mark} {title}</b>{"<br>" + failure if failure else ""}', color))
            # The fuller C++ suite covers isolation and repeated presses too.
            self.checks.value = ''.join(results) + '<hr>' + self.suite_html()
        except asyncio.CancelledError:
            pass
        finally:
            for control in self.controls:
                control.disabled = self.closed

    def close(self):
        self.closed = True
        for task in (self.task, self.clock_task):
            if task:
                task.cancel()
        self.session.close()
        for control in self.controls:
            control.disabled = True


def deactivate(name):
    previous = PANELS.pop(name, None)
    if previous:
        previous.close()
        previous.status.value = '此面板已停用；请使用重跑代码格后出现的新面板。'


def show_panel(lab, name, profile=None):
    deactivate(name)
    panel = PeripheralPanel(lab, profile)
    PANELS[name] = panel
    display(panel.view)
    return panel


@atexit.register
def close_panels():
    for panel in list(PANELS.values()):
        panel.close()
