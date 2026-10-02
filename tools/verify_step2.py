"""Check course integrity, execute presentation cells, or build ESP-IDF fixtures.

No port is accepted: this maintainer tool never flashes a device.
"""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

from academy_tutor import ROOT, load_course, step_content


def integrity():
    course = load_course()
    assert [x["id"] for x in course["lessons"]] == [f"H{x:02}" for x in range(15)]
    steps = 0
    for lesson in course["lessons"]:
        notebook = json.loads((ROOT / lesson["notebook"]).read_text(encoding="utf-8"))
        assert notebook["cells"][0]["cell_type"] == "markdown"
        assert ".academy/course.json" in "".join(notebook["cells"][0]["source"])
        assert notebook["metadata"]["academy"]["hardware_validated"] is False
        ids = [x["id"] for x in lesson["steps"]]
        assert len(ids) == len(set(ids))
        for step in lesson["steps"]:
            assert step["mode"] in {"Guided", "Modify", "Debug", "Build", "Open"}
            assert step_content(lesson["id"], step["id"])
            for config in step.get("configuration_sources", []):
                assert (ROOT / config["source"]).is_file()
            for example in step["examples"]:
                source = (ROOT / example["source"]).read_text(encoding="utf-8")
                cells = [c for c in notebook["cells"] if c["cell_type"] == "code"
                         and c.get("metadata", {}).get("academy", {}).get("step_id") == step["id"]]
                assert any("".join(c["source"]).split("\n", 1)[-1] == source for c in cells)
                assert "solution" not in example["source"]
            steps += 1
        for cell in notebook["cells"]:
            if cell["cell_type"] == "code":
                assert cell["execution_count"] is None and not cell["outputs"]
    print(f"[PASS] 15 Notebook / {steps} steps: references, example parity, clean outputs")


def notebooks():
    import nbformat
    from nbclient import NotebookClient
    from jupyter_client.kernelspec import KernelSpecManager
    from jupyter_client.manager import AsyncKernelManager
    with tempfile.TemporaryDirectory(prefix="academy-hardware-kernel-") as temporary:
        spec = Path(temporary) / "academy"
        spec.mkdir()
        (spec / "kernel.json").write_text(json.dumps({
            "argv": [sys.executable, "-m", "ipykernel_launcher", "-f", "{connection_file}"],
            "display_name": "Academy hardware check", "language": "python"}))
        manager = KernelSpecManager(kernel_dirs=[temporary])
        for lesson in load_course()["lessons"]:
            path = ROOT / lesson["notebook"]
            notebook = nbformat.read(path, as_version=4)
            nbformat.validate(notebook)
            kernel = AsyncKernelManager(kernel_name="academy", kernel_spec_manager=manager)
            NotebookClient(notebook, km=kernel, timeout=90,
                           resources={"metadata": {"path": str(path.parent)}}).execute()
            print(f"[PASS] {path.name}: real Jupyter presentation cells (no firmware execution)")


def firmware():
    idf_path = Path(os.environ.get("IDF_PATH", ""))
    if not (idf_path / "tools/idf.py").is_file():
        raise RuntimeError("先加载 ESP-IDF v5.5.1 环境，再使用 --firmware")
    version = subprocess.check_output([sys.executable, str(idf_path / "tools/idf.py"),
                                       "--version"], text=True).strip()
    if "v5.5.1" not in version:
        raise RuntimeError(f"构建基线要求 v5.5.1，当前 {version}")
    base = ROOT / ".build/step2-check"
    base.mkdir(parents=True, exist_ok=True)
    # Matrix compiles every Guided translation unit against real IDF headers/libraries.
    # Rename only entry symbols so they can coexist. Functions are not run.
    matrix = base / "matrix"
    main = matrix / "main"
    main.mkdir(parents=True, exist_ok=True)
    shutil.copy(ROOT / "step2/device-template/CMakeLists.txt", matrix)
    (main / "main.cpp").write_text('extern "C" void app_main() {}\n')
    paths = sorted((ROOT / "step2/lessons").glob("H*/examples/*.cpp"))
    registrations = []
    for index, path in enumerate(paths):
        lesson_id = path.parents[1].name
        name = f"{lesson_id}_{path.name}"
        shutil.copy(path, main / name)
        if 'extern "C" void app_main()' in path.read_text():
            registrations.append(f'set_source_files_properties("{name}" PROPERTIES '
                                 f'COMPILE_DEFINITIONS "app_main=check_entry_{index}")')
    for path in (ROOT / "step2/lessons").glob("H*/examples/*.hpp"):
        shutil.copy(path, main / path.name)
    shutil.copy(ROOT / "step2/lessons/H07/examples/Kconfig.projbuild", main)
    shutil.copy(ROOT / "step2/lessons/H13/examples/partitions.csv", matrix)
    sources = ' '.join('"' + f"{p.parents[1].name}_{p.name}" + '"' for p in paths)
    (main / "CMakeLists.txt").write_text(
        f'idf_component_register(SRCS "main.cpp" {sources} INCLUDE_DIRS "." REQUIRES '
        'esp_driver_gpio esp_driver_ledc esp_adc esp_driver_uart esp_driver_i2c '
        'esp_driver_spi esp_timer esp_wifi esp_event esp_netif esp_http_server mqtt '
        'wifi_provisioning nvs_flash esp_https_ota app_update esp_http_client mbedtls freertos log)\n'
        + '\n'.join(registrations) + '\n')
    (matrix / "sdkconfig.defaults").write_text(
        'CONFIG_FREERTOS_HZ=100\nCONFIG_HTTPD_WS_SUPPORT=y\n'
        'CONFIG_BT_ENABLED=y\nCONFIG_BT_NIMBLE_ENABLED=y\n'
        'CONFIG_BTDM_CTRL_MODE_BLE_ONLY=y\nCONFIG_BT_BLUEDROID_ENABLED=n\n'
        'CONFIG_PARTITION_TABLE_CUSTOM=y\nCONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions.csv"\n'
        'CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y\nCONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE=y\n'
        'CONFIG_MBEDTLS_CERTIFICATE_BUNDLE=y\n')
    projects = [("matrix", matrix)]
    for name in ("device-template", "guided-device"):
        target = base / name
        shutil.copytree(ROOT / "step2" / name, target, dirs_exist_ok=True,
                        ignore=shutil.ignore_patterns("build", "sdkconfig"))
        projects.append((name, target))
    for name, path in projects:
        logfile = base / f"{name}.log"
        print(f"[BUILD] {name}; log={logfile}", flush=True)
        with logfile.open("w") as log:
            result = subprocess.run([sys.executable, str(idf_path / "tools/idf.py"),
                                     "-C", str(path), "build"], stdout=log, stderr=subprocess.STDOUT)
        if result.returncode:
            print(logfile.read_text()[-7000:])
            raise RuntimeError(f"ESP-IDF build failed: {name}")
        print(f"[PASS] {name}: {version}, target esp32, compile/link only", flush=True)
    print(f"[PASS] {len(paths)} Guided C++ translation units compiled/linked; hardware NOT tested")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--notebooks", action="store_true")
    parser.add_argument("--firmware", action="store_true")
    args = parser.parse_args()
    integrity()
    if args.notebooks:
        notebooks()
    if args.firmware:
        firmware()


if __name__ == "__main__":
    main()
