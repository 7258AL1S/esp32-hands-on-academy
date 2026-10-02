"""One setup cell registers C++ lesson cells for the standard Python kernel."""
from pathlib import Path
import sys

# Works from a VS Code workspace root or a notebook subdirectory.
for candidate in (Path.cwd(), *Path.cwd().parents):
    if (candidate / "tools/notebook_lab.py").is_file():
        sys.path.insert(0, str(candidate / "tools"))
        break
else:
    raise FileNotFoundError("请在 esp32-hands-on-academy 项目目录中打开 Notebook")

import shlex
from IPython import get_ipython
from IPython.core.magic import register_cell_magic, register_line_magic
from notebook_lab import compile_lab
from notebook_widgets import show_panel


@register_cell_magic
def academy_lab(line, cell):
    """%%academy_lab <lesson-slug> <name> [guided|exercise] [pass|fail]."""
    from device_lab import compile_device, show_device
    args = shlex.split(line)
    if len(args) not in (2, 3, 4):
        raise ValueError('Usage: %%academy_lab <lesson-slug> <name> [guided|exercise] [pass|fail]')
    if len(args) >= 3 and args[2] not in {'guided', 'exercise'}:
        raise ValueError('Checks: guided or exercise')
    if len(args) == 4 and args[3] not in {'pass', 'fail'}:
        raise ValueError('Expected result: pass or fail')
    slug, name = args[:2]
    lab, spec = compile_device(slug, name, cell)
    profile = args[2] if len(args) >= 3 else None
    view = show_device(lab, spec, slug, name, profile)
    get_ipython().user_ns.setdefault('academy_devices', {})[(slug, name)] = view
    if profile:
        view.show_suite()
        expected = None if len(args) == 3 else args[3] == 'pass'
        lab.check(profile, expect_pass=expected, quiet=True)


@register_cell_magic
def cpp_lab(line, cell):
    """%%cpp_lab <name> [guided|exercise] [pass|fail] compiles this exact C++."""
    args = shlex.split(line)
    if len(args) not in (1, 2, 3):
        raise ValueError("Usage: %%cpp_lab <name> [guided|exercise] [pass|fail]")
    name = args[0]
    if len(args) >= 2 and args[1] not in {"guided", "exercise"}:
        raise ValueError("Checks: guided or exercise")
    if len(args) == 3 and args[2] not in {"pass", "fail"}:
        raise ValueError("Expected result: pass or fail")
    lab = compile_lab(name, cell, '#include "controller.hpp"\n')
    profile = args[1] if len(args) >= 2 else None
    panel = show_panel(lab, name, profile)
    # Available for verification; students interact with the visible controls.
    get_ipython().user_ns.setdefault("academy_panels", {})[name] = panel
    if len(args) >= 2:
        expected = None if len(args) == 2 else args[2] == "pass"
        panel.show_suite()
        lab.check(args[1], expect_pass=expected, quiet=True)


@register_line_magic
def esp32_board(line):
    from notebook_hardware import show_hardware
    show_hardware()


# Optional system labs register only their small, lesson-specific controls.
for _module_name in ('system_concurrency', 'system_config', 'system_ota'):
    try:
        _module = __import__(_module_name)
        _module.register()
    except ImportError:
        pass


print("✓ C++ 实验格已准备好。后面的代码使用 C++，无需学习 Python。")
