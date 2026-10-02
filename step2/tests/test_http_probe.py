"""Probe logic with localhost fixtures; these are not ESP32 hardware tests."""
from contextlib import contextmanager
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import json
import sys
import threading
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools.check_device import check_device


@contextmanager
def server(mode):
    class Handler(BaseHTTPRequestHandler):
        uptime = 0
        def do_GET(self):
            if self.path != '/api/status':
                self.send_error(404)
                return
            Handler.uptime += 100
            self.send_response(200)
            self.send_header('Content-Type', 'text/plain' if mode == 'type' else 'application/json')
            self.end_headers()
            value = True if mode == 'boolean' else (0 if mode == 'stale' else Handler.uptime)
            self.wfile.write(json.dumps({'uptime_ms': value}).encode())
        def log_message(self, *_):
            pass
    http = ThreadingHTTPServer(('127.0.0.1', 0), Handler)
    worker = threading.Thread(target=http.serve_forever, daemon=True)
    worker.start()
    try:
        yield f'http://127.0.0.1:{http.server_port}'
    finally:
        http.shutdown()
        http.server_close()
        worker.join()


class ProbeTests(unittest.TestCase):
    def test_live_response(self):
        with server('valid') as url:
            self.assertEqual(check_device(url), [100, 200])

    def test_bad_responses_are_not_passed(self):
        for mode in ('stale', 'type', 'boolean'):
            with self.subTest(mode=mode), server(mode) as url:
                with self.assertRaises(ValueError):
                    check_device(url)

    def test_invalid_url(self):
        with self.assertRaises(ValueError):
            check_device('file:///tmp/status')


if __name__ == '__main__':
    unittest.main()
