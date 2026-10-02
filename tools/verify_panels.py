"""Integration checks for the real C++ process and notebook widget callbacks."""
import asyncio
from pathlib import Path
from notebook_lab import Lab
from notebook_widgets import PeripheralPanel
from notebook_theme import FAILURE

ROOT = Path(__file__).resolve().parents[1]


def lab(name):
    return Lab(ROOT / '.build/notebook' / name / 'build')


async def main():
    panels = []
    try:
        guided = PeripheralPanel(lab('guided'), 'guided')
        panels.append(guided)
        guided.button.value = True  # Exercise the actual widget callback.
        assert guided.last.pressed and guided.last.led
        guided.button.value = False
        assert not guided.last.led
        print('[PASS] Button callbacks reach C++ and drive the visible LED')

        broken = PeripheralPanel(lab('broken'), 'exercise')
        panels.append(broken)
        broken.button.value = True
        assert broken.last.led
        broken.step('tick')
        assert not broken.last.led and broken.first_failure == (20, True, False)
        assert FAILURE in broken.status.value
        print('[PASS] Continued sampling exposes the real long-hold bug in red')
        broken.reset()
        assert broken.first_failure is None and not broken.last.led
        print('[PASS] Reset clears Controller and failure state')

        solution = PeripheralPanel(lab('solution'), 'exercise')
        panels.append(solution)
        solution.button.value = True
        for _ in range(30):
            assert solution.step('tick')
        assert solution.last.led and solution.first_failure is None
        # A separately running guided panel remains released / OFF.
        assert not guided.last.led
        solution.reset(held=True)
        assert not solution.last.led
        solution.button.value = False
        solution.button.value = True
        assert solution.last.led
        print('[PASS] Live sessions preserve long holds, boot baseline and isolation')

        await broken.animate_checks(delay=0)
        assert '✗ 保持按住' in broken.checks.value
        await solution.animate_checks(delay=0)
        assert '✗' not in solution.checks.value
        assert '[FAIL]' not in solution.checks.value
        print('[PASS] Animation detects broken C++ and accepts solution C++')

        solution.clock.value = True
        before = solution.last.time_ms
        await asyncio.sleep(.3)
        assert solution.last.time_ms > before
        solution.clock.value = False
        print('[PASS] Slow-motion clock actually advances C++ sampling')
    finally:
        for panel in panels:
            panel.close()
        assert all(panel.session.process.poll() is not None for panel in panels)
        print('[PASS] All simulator processes close cleanly')


if __name__ == '__main__':
    asyncio.run(main())
