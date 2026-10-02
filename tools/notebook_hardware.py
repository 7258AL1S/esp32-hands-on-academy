"""Small pin explorer and circuit calculator before the first GPIO experiment."""
import ipywidgets as widgets
from IPython.display import display
from notebook_widgets import Observation, board_svg
from notebook_theme import SUCCESS, FAILURE, html, panel

PIN_NOTES = {
    '认识板卡': 'ESP32 是芯片家族；ESP32-WROOM 是把芯片、Flash、天线等组合起来的模组；DevKitC 是带 USB、稳压和排针的开发板。本课选经典 ESP32 + DevKitC V4 作参考，S3/C3 等型号不能照抄引脚。',
    'GPIO18 / LED': '本课示例输出针脚。GPIO18 输出 HIGH，经 330 Ω 电阻和正接的外部 LED 回到 GND。不是开发板电源指示灯；GPIO18 也不等于排针第 18 个孔。',
    'GPIO19 / Button': '本课示例输入针脚。配置为上拉输入；按钮一端接 GPIO19，另一端接 GND。松开读 HIGH，按下读 LOW。模拟接口里的 LOW 约定源于这条接线。',
    '3V3 / GND': '3V3 是 3.3 V 电源，GND 是参考零电位和电流回路的一部分。GPIO 是信号端，不是通用电源。电路两端要有共同的参考地。',
    '5V / USB': 'USB 可向板卡提供 5 V，板上稳压器供给 3.3 V。5 V 电源针脚不意味着 GPIO 可接 5 V 信号；ESP32 GPIO 使用 3.3 V 逻辑。DevKitC 供电方式要择一。',
    'EN / BOOT': 'EN 复位芯片；BOOT 配合复位进入下载模式，经典 ESP32 通常涉及 GPIO0。它不是本课的外部业务按钮。',
    '其他针脚先注意什么': '经典 ESP32 的 GPIO6–11 通常连接 Flash，不用作本课 I/O；GPIO34–39 只输入且没有内部上拉/下拉；GPIO0/2/5/12/15 涉及启动配置。具体以芯片数据手册、模组和开发板原理图为准。',
}


def devkitc_function_map():
    """Function groups, not a universal physical pinout."""
    return '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 700 250" width="100%" font-family="system-ui, sans-serif" role="img" aria-label="ESP32 DevKitC 功能分组示意图">
    <rect x="2" y="2" width="696" height="246" rx="14" fill="#141414" stroke="#798491" stroke-width="2"/>
    <rect x="260" y="20" width="180" height="210" rx="12" fill="#174952"/>
    <rect x="299" y="36" width="102" height="38" rx="4" fill="#aeb8c1"/>
    <text x="350" y="60" text-anchor="middle" font-size="14" fill="#1b2b34">ESP32-WROOM</text>
    <text x="350" y="101" text-anchor="middle" font-size="13" fill="white">DevKitC 功能分组</text>
    <path d="M260 37 H204" stroke="#8cc8ff" stroke-width="6"/><text x="194" y="42" text-anchor="end" font-size="14" fill="#8cc8ff">USB / 5V</text>
    <path d="M260 67 H204" stroke="#ff7b86" stroke-width="6"/><text x="194" y="72" text-anchor="end" font-size="14" fill="#ff7b86">EN / BOOT</text>
    <path d="M260 125 H204" stroke="#65d6ae" stroke-width="6"/><text x="194" y="130" text-anchor="end" font-size="14" fill="#65d6ae">GPIO18 · 输出示例</text>
    <path d="M260 155 H204" stroke="#65d6ae" stroke-width="6"/><text x="194" y="160" text-anchor="end" font-size="14" fill="#65d6ae">GPIO19 · 输入示例</text>
    <path d="M440 50 H496" stroke="#ffe09a" stroke-width="6"/><text x="506" y="55" font-size="14" fill="#ffe09a">3V3</text>
    <path d="M440 82 H496" stroke="#b8c2cc" stroke-width="6"/><text x="506" y="87" font-size="14" fill="#eeeeee">GND</text>
    <path d="M440 130 H496" stroke="#ff7b86" stroke-width="6"/><text x="506" y="125" font-size="14" fill="#ff7b86">GPIO0/2/5/12/15</text><text x="506" y="144" font-size="14" fill="#ff7b86">涉及启动配置</text>
    <path d="M440 174 H496" stroke="#ff7b86" stroke-width="6"/><text x="506" y="171" font-size="14" fill="#ff7b86">GPIO6–11</text><text x="506" y="190" font-size="14" fill="#ff7b86">通常连接 Flash</text>
    <path d="M440 214 H496" stroke="#ff7b86" stroke-width="6"/><text x="506" y="219" font-size="14" fill="#ff7b86">GPIO34–39 · 仅输入</text>
    </svg>'''


def show_hardware():
    selector = widgets.Dropdown(options=list(PIN_NOTES), description='查看部件',
                                layout=widgets.Layout(width='100%', max_width='400px', min_width='0'))
    explanation = html()
    resistance = widgets.IntSlider(value=330, min=0, max=1000, step=10,
                                    description='电阻 Ω', continuous_update=False,
                                    layout=widgets.Layout(width='100%', max_width='480px', min_width='0'))
    current = html()

    def describe(_=None):
        explanation.value = '<p>' + PIN_NOTES[selector.value] + '</p>'

    def calculate(_=None):
        if resistance.value == 0:
            current.value = f'<b style="color:{FAILURE}">✗ 缺少限流电阻，不能直接将 LED 跨接输出与 GND。</b>'
        else:
            value = (3.3 - 2.0) / resistance.value * 1000
            color = FAILURE if value > 10 else SUCCESS
            prefix = '⚠ ' if value > 10 else '✓ '
            current.value = f'<b style="color:{color}">{prefix}估算 LED 电流：{value:.2f} mA</b><br>假定红色 LED 正向压降为 2.0 V：I = (3.3 − 2.0) / R。这是教学近似；本面板以 10 mA 为提醒阈值，不代表 GPIO 的额定值。真实电路请查芯片数据手册和 LED 额定值。'

    selector.observe(describe, names='value')
    resistance.observe(calculate, names='value')
    describe()
    calculate()
    display(panel([
        html('<div class="academy-title">认识板卡 · 针脚探索</div><p class="academy-note">参考板卡：经典 ESP32 + DevKitC V4。</p>'),
        html(devkitc_function_map()),
        html('<p class="academy-note">这是功能分组示意，不是完整 pinout。真实接线请查对应型号的官方针脚表和 datasheet。</p>'),
        selector, explanation,
        html('<b>本课接线 · 外部 LED 与按钮</b>'),
        html(board_svg(Observation(0, False, False))),
        html('<p class="academy-note">GPIO18 → 330 Ω → LED → GND。GPIO19 内部上拉至 3V3；按钮按下时连接 GND。这里展示接线原理，按钮操作在后面的 C++ 实验台。</p>'),
        html('<b>动一下电阻，观察限流</b>'), resistance, current,
        html('<a href="https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html">Espressif 官方 DevKitC V4 板卡图与 J1/J3 针脚表</a>'),
    ]))
