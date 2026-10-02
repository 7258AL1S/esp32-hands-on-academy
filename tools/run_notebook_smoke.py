#!/usr/bin/env python3
"""Validate and execute both notebooks in independent real Jupyter kernels."""
from pathlib import Path
import tempfile
import nbformat
from nbclient import NotebookClient
from jupyter_client.kernelspec import KernelSpecManager
from jupyter_client.manager import AsyncKernelManager
import json
import sys
import argparse

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(
        description="在独立 Jupyter kernel 中验证 Notebook；默认不改写课程文件。"
    )
    parser.add_argument(
        "--save",
        action="store_true",
        help="将执行输出保存回 Notebook；日常验证不要使用。",
    )
    args = parser.parse_args()
    # Use this interpreter explicitly: no dependence on a user's global kernels.
    with tempfile.TemporaryDirectory(prefix="academy-kernel-") as temporary:
        kernels = Path(temporary)
        spec = kernels / "academy"
        spec.mkdir()
        (spec / "kernel.json").write_text(json.dumps({
            "argv": [sys.executable, "-m", "ipykernel_launcher", "-f", "{connection_file}"],
            "display_name": "Academy validation", "language": "python",
        }))
        manager = KernelSpecManager(kernel_dirs=[str(kernels)])
        for path in sorted((ROOT / "notebooks").rglob("*.ipynb")):
            notebook = nbformat.read(path, as_version=4)
            nbformat.validate(notebook)
            kernel = AsyncKernelManager(kernel_name="academy", kernel_spec_manager=manager)
            client = NotebookClient(notebook, km=kernel, timeout=180,
                                    resources={"metadata": {"path": str(path.parent)}})
            client.execute(cleanup_kc=True)
            if args.save:
                nbformat.write(notebook, path)
            cells = [cell for cell in notebook.cells if cell.cell_type == "code"]
            errors = [output for cell in cells for output in cell.outputs
                      if output.output_type == "error"]
            if errors:
                raise RuntimeError(f"Unexpected errors: {path}")
            suffix = " and saved" if args.save else " (outputs kept in memory)"
            print(f"[PASS] {path.relative_to(ROOT)}: {len(cells)} cells executed{suffix}")


if __name__ == "__main__":
    main()
