"""An actual package integrity and rollback sandbox for lesson 24."""
from __future__ import annotations

from pathlib import Path
import shutil
import subprocess

import ipywidgets as widgets
from IPython.display import display
from notebook_theme import FAILURE, SUCCESS, controls, html, panel, check_card

ROOT = Path(__file__).resolve().parents[1]
SANDBOX = ROOT / ".academy-data" / "ota-lab"


def _compile() -> Path:
    source = ROOT / ".academy-data" / "ota-lab.cpp"; binary = ROOT / ".academy-data" / "ota-lab"
    source.parent.mkdir(parents=True, exist_ok=True)
    source.write_text(r'''#include "academy/system/ota_sandbox.hpp"
    #include <iostream>
    int main(int argc,char**argv){ academy::system::OtaSandbox ota(argv[1]); auto r=std::string(argv[2])=="stage"?ota.stage(argv[3]):std::string(argv[2])=="activate"?ota.activate():ota.boot(std::string(argv[3])=="healthy"); std::cout<<r.ok<<"|"<<r.state<<"|"<<r.detail<<"|slot="<<ota.current_slot(); }''', encoding="utf-8")
    subprocess.run(["c++", "-std=c++17", "-I", str(ROOT / "include"), str(source), "-o", str(binary)], check=True, capture_output=True, text=True)
    return binary


def _package(tampered: bool) -> Path:
    package = SANDBOX / "package"; package.mkdir(parents=True, exist_ok=True)
    image = package / "firmware.bin"; image.write_bytes(b"academy firmware v2\n")
    # Obtain the actual digest via the C++ SHA-256 used by staging.
    helper = ROOT / ".academy-data" / "digest.cpp"; exe = ROOT / ".academy-data" / "digest"
    helper.write_text('#include "academy/system/ota_sandbox.hpp"\n#include <iostream>\nint main(int c,char**v){std::cout<<academy::system::Sha256::file_hex(v[1]);}', encoding="utf-8")
    subprocess.run(["c++","-std=c++17","-I",str(ROOT/"include"),str(helper),"-o",str(exe)],check=True,capture_output=True,text=True)
    digest=subprocess.run([str(exe),str(image)],check=True,capture_output=True,text=True).stdout
    if tampered: digest = "0" * 64
    (package / "manifest.txt").write_text(f"version=2.0\nsha256={digest}\n", encoding="utf-8")
    return package


def show_ota_sandbox():
    verified = widgets.Button(description="生成并校验包", icon="check")
    broken = widgets.Button(description="生成篡改包", icon="warning")
    activate = widgets.Button(description="激活候选槽", icon="play")
    healthy = widgets.Button(description="健康启动", icon="check-circle")
    failed = widgets.Button(description="失败启动", icon="undo")
    clear = widgets.Button(description="清空沙箱", icon="trash")
    output = html()
    def invoke(action, arg):
        try:
            binary = _compile(); result=subprocess.run([str(binary),str(SANDBOX/"device"),action,str(arg)],check=True,capture_output=True,text=True).stdout
            color=SUCCESS if result.startswith("1|") else FAILURE
            output.value=check_card(f'<b style="color:{color}">{result}</b>', color)
        except Exception as error: output.value=check_card(f'<b style="color:{FAILURE}">✗ {error}</b>', FAILURE)
    verified.on_click(lambda _: invoke("stage", _package(False)))
    broken.on_click(lambda _: invoke("stage", _package(True)))
    activate.on_click(lambda _: invoke("activate", "-"))
    healthy.on_click(lambda _: invoke("boot", "healthy"))
    failed.on_click(lambda _: invoke("boot", "failed"))
    clear.on_click(lambda _: (shutil.rmtree(SANDBOX, ignore_errors=True), setattr(output,"value","已清空 OTA 沙箱。")))
    display(panel([
        html('<div class="academy-title">Host OTA 安全沙箱</div><p class="academy-note">按钮操作真实创建包、计算 SHA-256、写入非活动槽并执行确认或回滚。它不写 ESP32 Flash，也不替代签名、secure boot、分区表或 bootloader 验证。</p>'),
        controls([verified, broken, activate]), controls([healthy, failed, clear]), output,
    ]))


def register():
    from IPython.core.magic import register_line_magic
    @register_line_magic
    def ota_sandbox(line):
        show_ota_sandbox()

