"""Real local configuration store visualized for lesson 22."""
from __future__ import annotations

import json
from pathlib import Path
import shutil
import subprocess

import ipywidgets as widgets
from IPython.display import display

from notebook_theme import FAILURE, SUCCESS, controls, html, panel, check_card

ROOT = Path(__file__).resolve().parents[1]
SANDBOX = ROOT / ".academy-data" / "config-lab"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8") if path.exists() else "（尚未写入）"


def show_config_store():
    name = widgets.Text(value="desk-notifier", description="设备名", layout=widgets.Layout(width="100%"))
    interval = widgets.IntText(value=60000, description="上报间隔 ms", layout=widgets.Layout(width="100%"))
    save = widgets.Button(description="写入配置", icon="save")
    migrate = widgets.Button(description="写入 v1 并迁移", icon="history")
    reset = widgets.Button(description="清空沙箱", icon="trash")
    status = html()
    contents = html()

    def refresh(message=""):
        contents.value = check_card('<b>真实本地文件</b><pre style="white-space:pre-wrap;color:#eeeeee;margin:6px 0">' + _read(SANDBOX / "device.conf") + "</pre>", SUCCESS)
        status.value = message

    def compile_and_run():
        source = ROOT / ".academy-data" / "config-lab.cpp"
        binary = ROOT / ".academy-data" / "config-lab"
        source.parent.mkdir(parents=True, exist_ok=True)
        source.write_text(r'''#include "academy/system/config_store.hpp"
        #include <iostream>
        int main(int argc,char**argv){ academy::system::ConfigStore store(argv[1]); store.save({2,argv[2],std::stoi(argv[3])}); auto value=store.load(); std::cout<<value.config.device_name; }''', encoding="utf-8")
        subprocess.run(["c++", "-std=c++17", "-I", str(ROOT / "include"), str(source), "-o", str(binary)], check=True, capture_output=True, text=True)
        return subprocess.run([str(binary), str(SANDBOX), name.value, str(interval.value)], check=True, capture_output=True, text=True).stdout

    def on_save(_):
        try:
            compile_and_run(); refresh(f'<b style="color:{SUCCESS}">✓ 已原子替换真实配置文件；重新执行此格仍会读到它。</b>')
        except Exception as error:
            refresh(f'<b style="color:{FAILURE}">✗ 没有写入：{error}</b>')

    def on_migrate(_):
        SANDBOX.mkdir(parents=True, exist_ok=True)
        (SANDBOX / "device.conf").write_text("version=1\nname=legacy-desk\ninterval_s=12\n", encoding="utf-8")
        # The test executable uses the same ConfigStore migration path.
        status.value = f'<b style="color:{SUCCESS}">✓ 已放入 v1 文件。运行下方测试将迁移并验证写回 v2。</b>'
        refresh(status.value)

    def on_reset(_):
        shutil.rmtree(SANDBOX, ignore_errors=True); refresh('<b>已清空本课沙箱。</b>')
    save.on_click(on_save); migrate.on_click(on_migrate); reset.on_click(on_reset)
    view = panel([
        html('<div class="academy-title">配置文件实验台</div><p class="academy-note">这是电脑本地文件沙箱，真正落盘，便于观察版本迁移。ESP32 对应实现通常是 NVS，不是把这个文件直接复制到 Flash。</p>'),
        controls([name, interval]), controls([save, migrate, reset]), status, contents,
    ])
    refresh(); display(view)


def register():
    from IPython.core.magic import register_line_magic
    @register_line_magic
    def config_store(line):
        show_config_store()

