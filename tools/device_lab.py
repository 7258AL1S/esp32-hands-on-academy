"""C++ cells and live peripheral panels for the shared host Device fixture."""
from __future__ import annotations
import asyncio
import atexit
from collections import deque
from html import escape
import json
from pathlib import Path
from queue import Queue, Empty
import re
import shutil
import subprocess
import threading

import ipywidgets as widgets
from IPython.display import display
from notebook_lab import Lab, ROOT
from notebook_theme import html, panel, controls, check_card, SUCCESS, FAILURE

PANELS = {}
DEFAULT_INPUT = dict(now_ms=0, button=False, analog=2048, temperature=22.0,
                     connected=True, fault=False, pulses=0, text='')
OUTPUT_KEYS = {'now_ms', 'led', 'brightness', 'reading', 'events', 'state',
               'outgoing', 'display', 'queue_depth', 'error'}


def compile_device(slug, name, code):
    if not re.fullmatch(r'[0-9]{2}-[a-z0-9-]+', slug) or not re.fullmatch(r'[a-z][a-z0-9_-]*', name):
        raise ValueError('Invalid lesson or experiment name')
    lesson = ROOT / 'lessons' / slug
    spec = json.loads((lesson / 'lab.json').read_text(encoding='utf-8'))
    previous = PANELS.pop((slug, name), None)
    if previous:
        previous.close()
        previous.status.value = '此面板已停用；请使用重跑本格后出现的新面板。'
    cmake = shutil.which('cmake')
    if not cmake:
        raise RuntimeError('找不到 CMake；请安装并重启 VS Code。')
    directory = ROOT / '.build/notebook' / slug / name
    source = directory / 'source'
    source.mkdir(parents=True, exist_ok=True)
    (source / 'controller.hpp').write_text(code, encoding='utf-8')
    (source / 'controller.cpp').write_text('#include "controller.hpp"\n', encoding='utf-8')
    build = directory / 'build'
    for command in ([cmake, '-S', str(ROOT), '-B', str(build), f'-DACADEMY_LESSON={slug}',
                     '-DLAB_VARIANT=starter', f'-DLAB_SOURCE_DIR={source}'],
                    [cmake, '--build', str(build), '--config', 'Debug', '--parallel', '2']):
        result = subprocess.run(command, capture_output=True, text=True, timeout=120)
        if result.returncode:
            print(result.stdout + result.stderr)
            raise RuntimeError('C++ 构建失败；阅读 compiler 诊断，修复本格代码后重跑。')
    print(f'✓ 已编译本格 C++：{slug} / {name}')
    return Lab(build), spec


class DeviceSession:
    def __init__(self, lab):
        self.process = subprocess.Popen([str(lab.executable('simulator'))],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, encoding='utf-8', bufsize=1)
        self.queue = Queue()
        self.reader = threading.Thread(target=self._read, daemon=True)
        self.reader.start()

    def _read(self):
        for line in self.process.stdout:
            self.queue.put(line)
        self.queue.put(None)

    def send(self, values=None):
        if values is None:
            command = 'reset'
        else:
            text_hex = str(values['text']).encode('utf-8').hex() or '-'
            command = f"{int(values['now_ms']) & 0xffffffff} {int(values['button'])} {int(values['analog'])} {float(values['temperature'])} {int(values['connected'])} {int(values['fault'])} {int(values['pulses'])} {text_hex}"
        self.process.stdin.write(command + '\n')
        self.process.stdin.flush()
        try:
            line = self.queue.get(timeout=3)
        except Empty:
            self.close()
            raise RuntimeError('C++ 在 3 秒内没有响应；检查阻塞或死循环。')
        try:
            observation = json.loads(line or '')
            if not OUTPUT_KEYS <= observation.keys():
                raise ValueError('missing observations')
        except (ValueError, AttributeError) as error:
            self.close()
            raise RuntimeError(f'C++ 输出异常：{line or "进程退出"}') from error
        return observation

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


def circuit_svg(kind, inputs, output, mismatch=False):
    """Keep labels short and all long explanations in wrapping HTML outside SVG."""
    color = FAILURE if mismatch or output['error'] else SUCCESS
    duty = max(0, min(255, output['brightness']))
    led = '#ffd24a' if output['led'] or duty else '#46515e'
    opacity = .25 + .75 * duty / 255 if duty else 1
    icons = {'button':'BUTTON', 'timer':'TIMER', 'memory':'BUFFER', 'pwm':'PWM', 'adc':'ADC',
             'sensor':'SENSOR', 'interrupt':'IRQ', 'uart':'UART', 'i2c':'I2C', 'spi':'SPI',
             'task':'TASK / QUEUE', 'config':'CONFIG', 'ble':'BLE MODEL', 'ota':'OTA MODEL'}
    label = icons.get(kind, 'DEVICE')
    value = ('LOW' if inputs['button'] else 'HIGH') if kind == 'button' else label
    return f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 600 170" role="img" aria-label="{escape(label)} 实验台，LED {'ON' if output['led'] else 'OFF'}" font-family="system-ui,sans-serif">
    <rect x="2" y="2" width="596" height="166" rx="14" fill="#141414" stroke="{color}" stroke-width="2"/>
    <rect x="24" y="24" width="190" height="120" rx="10" fill="#153f47"/>
    <rect x="53" y="43" width="132" height="44" rx="4" fill="#adb7c0"/>
    <text x="119" y="72" text-anchor="middle" fill="#16252f" font-size="17">Virtual ESP32</text>
    <text x="119" y="120" text-anchor="middle" fill="#eeeeee" font-size="15">C++ Controller</text>
    <path d="M214 65 H288 M214 122 H288" fill="none" stroke="{color}" stroke-width="3"/>
    <rect x="288" y="99" width="150" height="46" rx="8" fill="#30343b" stroke="#798491"/>
    <text x="363" y="128" text-anchor="middle" fill="#eeeeee" font-size="15">{value}</text>
    <circle cx="322" cy="65" r="21" fill="{led}" fill-opacity="{opacity:.2f}" stroke="#b8c2cc" stroke-width="3"/>
    <text x="365" y="70" fill="#eeeeee" font-size="16">LED {'ON' if output['led'] else 'OFF'}</text>
    <text x="465" y="122" fill="#b8c2cc" font-size="14">{'LINK UP' if inputs['connected'] else 'LINK DOWN'}</text>
    </svg>'''


def waveform(history, kind):
    if len(history) < 2:
        return '<p class="academy-note">操作输入或推进时间后，这里显示实际采样波形。</p>'
    points = list(history)
    lanes = [('button', '原始按钮', True), ('led', 'LED', True)]
    if kind in {'adc', 'sensor', 'pwm'}:
        lanes = [('analog', 'ADC 原始值', False), ('reading', 'C++ 换算值', False)]
    elif kind in {'task', 'interrupt'}:
        lanes = [('events', '已处理事件', False), ('queue_depth', '队列深度', False)]
    lines = []
    for lane, (key, title, binary) in enumerate(lanes):
        values = [float(p.get(key, 0)) for p in points]
        low, high = (0, 1) if binary else (min(values), max(values))
        if high == low:
            high = low + 1
        top = 20 + lane * 75
        coords = []
        for i, value in enumerate(values):
            x = 18 + i * 564 / (len(values) - 1)
            y = top + 45 - 35 * (value - low) / (high - low)
            if binary and coords:
                coords.append((x, coords[-1][1]))
            coords.append((x, y))
        path = ' '.join(f'{x:.1f},{y:.1f}' for x,y in coords)
        lines.append(f'<polyline points="{path}" fill="none" stroke="{SUCCESS if lane == 0 else "#8cc8ff"}" stroke-width="2"/>')
    caption = ' · '.join(title for _,title,_ in lanes)
    return f'<p class="academy-note">{caption}（上 / 下轨，最近 {len(points)} 次采样）</p><svg viewBox="0 0 600 155" role="img" aria-label="采样波形"><rect width="600" height="155" rx="8" fill="#141414"/>{"".join(lines)}</svg>'


class DevicePanel:
    def __init__(self, lab, spec, profile=None):
        self.lab, self.spec, self.profile = lab, spec, profile
        self.kind = spec.get('kind', 'button')
        self.inputs = dict(DEFAULT_INPUT)
        self.history = deque(maxlen=100)
        self.closed = False
        self.task = self.clock_task = None
        self.first_failure = None
        self.syncing = False
        self.session = DeviceSession(lab)
        self.diagram, self.wave, self.status, self.checks, self.details = [html() for _ in range(5)]
        self.button = widgets.ToggleButton(description='按下按钮', layout=widgets.Layout(width='150px'))
        self.analog = widgets.IntSlider(description='ADC / duty', min=0, max=4095, value=2048,
                          continuous_update=False, layout=widgets.Layout(width='100%', max_width='460px'))
        self.temperature = widgets.FloatSlider(description='温度 °C', min=-20,max=80,value=22,
                          continuous_update=False, layout=widgets.Layout(width='100%', max_width='460px'))
        self.connected = widgets.ToggleButton(value=True,description='已连接',layout=widgets.Layout(width='130px'))
        self.fault = widgets.ToggleButton(description='注入故障',layout=widgets.Layout(width='140px'))
        self.pulses = widgets.BoundedIntText(description='脉冲计数', min=0,max=1000000,
                                           layout=widgets.Layout(width='240px'))
        self.text = widgets.Textarea(description='输入 / 命令', placeholder=spec.get('placeholder','输入后点击发送'),
                                    layout=widgets.Layout(width='100%',height='78px'))
        send = widgets.Button(description='发送输入',icon='send')
        tick = widgets.Button(description=f'采样 +{spec.get("step_ms",10)} ms',icon='step-forward')
        self.clock = widgets.ToggleButton(description='连续采样（慢放）',icon='play',layout=widgets.Layout(width='185px'))
        reset = widgets.Button(description='复位',icon='refresh')
        check = widgets.Button(description='播放自动检查',icon='check',button_style='info')
        self.controls = [self.button,self.analog,self.temperature,self.connected,self.fault,self.pulses,self.text,send,tick,self.clock,reset,check]
        for item in (self.button,self.connected,self.fault,self.clock):
            item.tooltip = '点击切换状态'
        send.tooltip='发送当前输入给本格 C++，随后清空瞬时命令'
        tick.tooltip='推进虚拟时钟并运行一次 C++ tick()'
        reset.tooltip='重新创建本格 C++ 控制器，清空观测和首次失败'
        check.tooltip='按测试场景逐步运行当前 C++ 并检查真实输出'
        selected = spec.get('inputs')
        if selected is None:
            selected = {'button':['button','fault'],'timer':['button','text'],
                        'memory':['text','fault'],'pwm':['analog'],'adc':['analog','fault'],
                        'sensor':['temperature','analog','connected','fault'],
                        'interrupt':['button','pulses','fault'],'uart':['text','connected','fault'],
                        'i2c':['text','temperature','connected','fault'],
                        'spi':['text','connected','fault'],'task':['text','button','fault'],
                        'config':['text','fault'],'ble':['text','connected','fault'],
                        'ota':['text','connected','fault']}.get(self.kind,['button'])
        children = [html(f'<div class="academy-title">{escape(spec["title"])} · 实验台</div><p class="academy-note">输入 → 本格 C++ → 实际观测。虚拟时间与硬件时序分开验证。</p>'),self.diagram]
        toggles=[getattr(self,k) for k in selected if k in {'button','connected','fault'}]
        if toggles: children.append(controls(toggles))
        for key in ('analog','temperature','pulses'):
            if key in selected: children.append(getattr(self,key))
        if 'text' in selected: children += [self.text,controls([send])]
        children += [controls([tick,self.clock]),controls([reset,check]),self.status,self.details,self.wave,self.checks]
        self.view=panel(children)
        self.button.observe(self._change_button,names='value')
        for name in ('analog','temperature','connected','fault','pulses'):
            getattr(self,name).observe(lambda change,key=name:self._change_input(key,change['new']),names='value')
        send.on_click(lambda _:self.submit())
        tick.on_click(lambda _:self.advance())
        reset.on_click(lambda _:self.reset())
        check.on_click(lambda _:self.start_checks())
        self.clock.observe(self._change_clock,names='value')
        self.render(self.session.send(self.inputs))

    def _change_button(self, change):
        self._change_input('button',change['new'])

    def _change_input(self,key,value):
        if not self.syncing and not self.closed:
            self.inputs[key]=value
            self.advance()

    def submit(self):
        self.inputs['text']=self.text.value
        self.advance()
        self.inputs['text']=''

    def render(self,output,expect=None):
        self.last=output
        differences=[]
        for key,wanted in (expect or {}).items():
            actual=output.get(key)
            good=abs(actual-wanted)<=1e-6 if isinstance(wanted,float) and isinstance(actual,(int,float)) else actual==wanted
            if not good: differences.append(f'{key}：预期 {wanted!r}，实际 {actual!r}')
        if differences and self.first_failure is None:
            self.first_failure=f't={self.inputs["now_ms"]} ms；'+'；'.join(differences)
        self.history.append({**self.inputs,**output})
        self.diagram.value=circuit_svg(self.kind,self.inputs,output,bool(differences))
        self.wave.value=waveform(self.history,self.kind)
        self.button.description='松开按钮' if self.inputs['button'] else '按下按钮'
        self.button.button_style='warning' if self.inputs['button'] else ''
        self.connected.description='已连接' if self.inputs['connected'] else '已断开'
        self.fault.button_style='warning' if self.inputs['fault'] else ''
        if self.first_failure:
            message=f'<b style="color:{FAILURE}">✗ 首次不符：{escape(self.first_failure)}。复位清除。</b>'
        else:
            message='<span class="academy-note">手动观察；播放自动检查后核对行为契约。</span>'
        self.status.value=f'<b>t={self.inputs["now_ms"]} ms</b> · {escape(str(output["state"]))}<br>{message}'
        self.details.value=f'<b>事件：{output["events"]} · 读值：{output["reading"]:g} · duty：{output["brightness"]} · Queue：{output["queue_depth"]}</b>'
        for title,key in [('显示器','display'),('通信输出','outgoing')]:
            if output[key]:
                self.details.value += f'<p><b>{title}</b><br><span style="white-space:pre-wrap">{escape(str(output[key]))}</span></p>'
        return not differences

    def advance(self, values=None, expect=None):
        if self.closed: return False
        if values is None:
            self.inputs['now_ms']=(self.inputs['now_ms']+self.spec.get('step_ms',10))&0xffffffff
        else:
            self.inputs.update(values)
            self.syncing=True
            try:
                for key in ('button','analog','temperature','connected','fault','pulses'):
                    getattr(self,key).value=self.inputs[key]
            finally: self.syncing=False
        try: return self.render(self.session.send(self.inputs),expect)
        except Exception as error:
            self.status.value=f'<b style="color:{FAILURE}">✗ 运行异常：{escape(str(error))}</b>'
            self.close()
            return False

    def reset(self):
        self.inputs=dict(DEFAULT_INPUT)
        self.first_failure=None
        self.history.clear()
        self.syncing=True
        try:
            for key in ('button','analog','temperature','connected','fault','pulses'):
                getattr(self,key).value=self.inputs[key]
        finally:self.syncing=False
        if not self.closed:
            self.render(self.session.send())

    async def _clock_loop(self):
        try:
            while self.clock.value and not self.closed:
                await asyncio.sleep(.25)
                self.advance()
        except asyncio.CancelledError: pass

    def _change_clock(self,change):
        if change['new']: self.clock_task=asyncio.create_task(self._clock_loop())
        elif self.clock_task:self.clock_task.cancel()

    def suite_html(self):
        result=self.lab.test_result(self.profile or 'exercise')
        if result.returncode not in (0,1): raise RuntimeError(result.stderr or 'C++ 测试进程异常')
        cards=[]
        for line in result.stdout.splitlines():
            if line.startswith(('[PASS]','[FAIL]')):
                color=SUCCESS if line.startswith('[PASS]') else FAILURE
                cards.append(check_card(f'<span style="color:{color}">{escape(line)}</span>',color))
            elif line.strip(): cards.append(f'<p>{escape(line)}</p>')
        return '<b>完整 C++ 行为检查</b>'+''.join(cards)

    def show_suite(self):self.checks.value=self.suite_html()

    def start_checks(self):
        self.clock.value=False
        if self.task and not self.task.done():return
        self.task=asyncio.create_task(self.animate_checks())

    async def animate_checks(self,delay=.2):
        scenarios=self.spec.get('scenarios',{}).get(self.profile or 'exercise',[])
        cards=[]
        for control in self.controls:control.disabled=True
        try:
            for scenario in scenarios:
                self.reset()
                good=True
                failure=''
                self.checks.value=''.join(cards)+f'<p>◌ 正在检查：{escape(scenario["title"])}</p>'
                for step in scenario['steps']:
                    passed=self.advance(step.get('input',{}),step.get('expect'))
                    if not passed and not failure:failure=self.first_failure or '运行异常'
                    good=passed and good
                    await asyncio.sleep(delay)
                color,mark=(SUCCESS,'✓') if good else (FAILURE,'✗')
                cards.append(check_card(f'<b style="color:{color}">{mark} {escape(scenario["title"])}</b>'+(f'<p>{escape(failure)}</p>' if failure else ''),color))
            self.checks.value=''.join(cards)+'<hr>'+self.suite_html()
        except asyncio.CancelledError:pass
        finally:
            for control in self.controls:control.disabled=self.closed

    def close(self):
        self.closed=True
        try: current=asyncio.current_task()
        except RuntimeError: current=None
        for task in (self.task,self.clock_task):
            if task and task is not current:task.cancel()
        self.session.close()
        for control in self.controls:control.disabled=True


def show_device(lab,spec,slug,name,profile):
    view=DevicePanel(lab,spec,profile)
    PANELS[(slug,name)]=view
    display(view.view)
    return view


@atexit.register
def cleanup():
    for view in list(PANELS.values()):view.close()
