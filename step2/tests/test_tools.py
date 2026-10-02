"""Safety of project creation and scope of teaching reads."""
from pathlib import Path
import json
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))
from tools.academy_tutor import init_project, step_content
from tools import esp32_notebook


class CourseTools(unittest.TestCase):
    def test_existing_student_code_is_never_overwritten(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'main.cpp'
            source.write_text('my work')
            with self.assertRaises(ValueError):
                init_project(root)
            self.assertEqual(source.read_text(), 'my work')
            self.assertEqual(list(root.iterdir()), [source])

    def test_new_project_is_independent_and_linked(self):
        with tempfile.TemporaryDirectory() as directory:
            target = init_project(Path(directory) / 'my-device')
            link = json.loads((target / '.academy/course.json').read_text())
            self.assertEqual(Path(link['course_root']), ROOT)
            self.assertEqual(link['start_lesson'], 'H02')
            self.assertIn('app_main()', (target / 'main/main.cpp').read_text())
            self.assertNotIn('wifi', (target / 'main/main.cpp').read_text())
            self.assertTrue((target / 'AGENTS.md').is_file())
            with self.assertRaises(ValueError):
                init_project(target)

    def test_guided_project_starts_at_h00(self):
        with tempfile.TemporaryDirectory() as directory:
            target = init_project(Path(directory) / 'guided', guided=True)
            self.assertIn('Lesson: H00', (target / '.academy/progress.md').read_text())

    def test_read_only_current_step_no_future_solution(self):
        source = step_content('H03', 'debug')
        self.assertIn('1000', source)
        self.assertNotIn('## 稳定按键', source)
        self.assertNotIn('solution.md', source)
        with self.assertRaises(ValueError):
            step_content('H03', 'nonexistent')
        with self.assertRaises(ValueError):
            step_content('../../secret', 'debug')

    def test_manual_check_requires_evidence_and_escapes_input(self):
        captured = []
        original = esp32_notebook.display
        esp32_notebook.display = captured.append
        try:
            esp32_notebook.hardware_checklist('H00', ['<script>test</script>'])
        finally:
            esp32_notebook.display = original
        panel = captured[0]
        row = panel.children[-1]
        status, checkbox, note = row.children
        self.assertIn('&lt;script&gt;', status.value)
        checkbox.value = True
        self.assertIn('未验证', status.value)
        note.value = 'observed output'
        self.assertIn('未自动测量', status.value)
        self.assertIn('✓', status.value)
        checkbox.value = False
        self.assertIn('未验证', status.value)


if __name__ == '__main__':
    unittest.main()
