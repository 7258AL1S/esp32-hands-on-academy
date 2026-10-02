"""Hardware Notebook presentation. Does not write, compile or flash firmware."""
from __future__ import annotations

import html
import shlex

import ipywidgets as widgets
from IPython.display import display
from tools import notebook_theme as theme


def install(ipython):
    if ipython is None:
        raise RuntimeError("请在 Jupyter Notebook 中运行初始化格")

    def example(line, cell):
        args = shlex.split(line)
        if len(args) != 2 or args[0] != "--target":
            raise ValueError("格式: %%esp32_example --target main/example.cpp")
        content = (
            f'<div class="academy-title">C++ 示例 · {html.escape(args[1])}</div>'
            '<p>这是示例预览，没有执行 C++、修改工程或烧录。'
            '请在 ESP-IDF 工程编辑目标文件，用官方命令运行；'
            '合并时保留已有功能与唯一的 app_main()。</p>'
            f'<pre style="white-space:pre;overflow-x:auto;max-width:100%;'
            f'color:{theme.TEXT};background:{theme.SURFACE};padding:12px">'
            f'{html.escape(cell)}</pre>'
        )
        display(theme.panel([theme.html(theme.STYLE + content)]))

    ipython.register_magic_function(example, "cell", "esp32_example")
    print("硬件示例格已准备好。C++ 在 ESP-IDF 工程构建；Notebook 不会自动烧录。")


def hardware_checklist(lesson_id, items):
    """Manual evidence cards. Checked items are explicitly learner observations."""
    rows = []
    for text in items:
        checkbox = widgets.Checkbox(description="我已实际观察并记录", value=False,
                                    indent=False, layout=widgets.Layout(width="100%"))
        evidence = widgets.Textarea(placeholder="操作、实际结果、日志/照片/测量位置；未确认请留空",
                                    layout=widgets.Layout(width="100%", height="80px"))
        status = theme.html(theme.check_card("○ 未验证：" + html.escape(text), theme.MUTED))

        def update(_change, check=checkbox, note=evidence, card=status, label=text):
            verified = check.value and bool(note.value.strip())
            color = theme.SUCCESS if verified else theme.MUTED
            message = "✓ 学习者记录（未自动测量）：" if verified else "○ 未验证："
            card.value = theme.check_card(message + html.escape(label), color)

        checkbox.observe(update, names="value")
        evidence.observe(update, names="value")
        rows.append(widgets.VBox([status, checkbox, evidence],
                                 layout=widgets.Layout(width="100%")))
    display(theme.panel([theme.html(theme.STYLE +
        f'<div class="academy-title">{html.escape(lesson_id)} · Hardware Checklist</div>'
        '<p>手动记录，不是自动硬件测试。勾选且填写证据后显示绿色；'
        '请将证据保存到工程 .academy/progress.md，重启 kernel 后这些控件会清空。</p>'),
        *rows]))
