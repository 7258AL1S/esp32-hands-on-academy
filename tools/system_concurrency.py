"""Real host-thread probes used by lessons 19-21.

They demonstrate the host's std::thread primitives. They deliberately do not
claim to be FreeRTOS: no ESP32 scheduler, interrupt context, task priorities,
or task stacks are simulated here.
"""
from __future__ import annotations

from concurrent.futures import ThreadPoolExecutor
import subprocess
import tempfile
from pathlib import Path
import textwrap

from IPython.display import display

from notebook_theme import FAILURE, SUCCESS, html, panel, check_card

ROOT = Path(__file__).resolve().parents[1]


def _run(source: str) -> str:
    with tempfile.TemporaryDirectory(prefix="academy-thread-") as directory:
        root = Path(directory)
        cpp = root / "probe.cpp"
        executable = root / "probe"
        cpp.write_text(textwrap.dedent(source), encoding="utf-8")
        compile_result = subprocess.run(
            ["c++", "-std=c++17", "-O2", "-pthread", str(cpp), "-o", str(executable)],
            text=True, capture_output=True, timeout=20)
        if compile_result.returncode:
            raise RuntimeError(compile_result.stderr)
        result = subprocess.run([str(executable)], text=True, capture_output=True, timeout=10)
        if result.returncode:
            raise RuntimeError(result.stderr)
        return result.stdout.strip()


def show_concurrency_probe():
    """Run bounded real threads; all joins are guaranteed."""
    def produce(index):
        return index + 1
    with ThreadPoolExecutor(max_workers=2) as executor:
        values = list(executor.map(produce, range(4)))
    output = _run(r'''
        #include <atomic>
        #include <iostream>
        #include <mutex>
        #include <queue>
        #include <thread>
        int main() {
          std::queue<int> queue; std::mutex lock; std::atomic<bool> ready{false};
          std::thread producer([&]{ std::lock_guard<std::mutex> guard(lock); queue.push(42); ready=true; });
          std::thread consumer([&]{ while (!ready.load()) {} std::lock_guard<std::mutex> guard(lock); std::cout << queue.front(); });
          producer.join(); consumer.join();
        }
    ''')
    view = panel([
        html('<div class="academy-title">Host 并发探针</div><p class="academy-note">这段代码真的使用 <code>std::thread</code> 和 <code>std::mutex</code>，随后 join；它不是 FreeRTOS，也没有模拟 ESP32 的优先级、栈、ISR 或双核调度。</p>'),
        html(check_card(f'<b style="color:{SUCCESS}">✓ 线程池结果</b><br>四个独立工作项：{values}', SUCCESS)),
        html(check_card(f'<b style="color:{SUCCESS}">✓ C++ producer / consumer</b><br>互斥锁保护的队列取到：{output}', SUCCESS)),
        html('<p class="academy-note">不要为了两个很短、严格顺序的动作而引入 Task、Queue 和锁。单一状态机或周期 tick 更容易证明正确。</p>'),
    ])
    display(view)


def show_race_probe():
    safe = _run(r'''
        #include <atomic>
        #include <iostream>
        #include <thread>
        int main() { std::atomic<int> count{0}; std::thread a([&]{for(int i=0;i<20000;++i) ++count;}); std::thread b([&]{for(int i=0;i<20000;++i) ++count;}); a.join(); b.join(); std::cout << count; }
    ''')
    view = panel([
        html('<div class="academy-title">Race 与同步</div><p class="academy-note">这个探针只运行无数据竞争的版本。未同步的共享 <code>int</code> 是 C++ 的未定义行为，不能把“这次看起来正常”当作实验结论。</p>'),
        html(check_card(f'<b style="color:{SUCCESS}">✓ atomic 计数</b><br>两个真实线程各递增 20,000 次，结果：{safe}', SUCCESS)),
        html(check_card(f'<b style="color:{FAILURE}">✗ 安全约束</b><br>死锁示例不在 Notebook 自动运行：它会占住学习环境。请用固定锁顺序或更少的共享资源来预防。</br>', FAILURE)),
    ])
    display(view)


def register():
    from IPython import get_ipython
    from IPython.core.magic import register_line_magic

    @register_line_magic
    def concurrency_probe(line):
        show_concurrency_probe()

    @register_line_magic
    def race_probe(line):
        show_race_probe()

