"""Scoped VS Code dark theme shared by every Academy peripheral panel."""
import ipywidgets as widgets

BACKGROUND = '#1e1e1e'
SURFACE = '#141414'
TEXT = '#eeeeee'
MUTED = '#b8c2cc'
SUCCESS = '#65d6ae'
FAILURE = '#ff7b86'

# Widgets live in the notebook renderer. Never style the notebook itself.
STYLE = '''<style>
/* VS Code's widget renderer adds a white output wrapper by default. */
.cell-output-ipywidget-background:has(.academy-panel) {
  background: #1e1e1e !important; color: #eeeeee;
}
.academy-panel {
  background: #1e1e1e; color: #eeeeee; border: 1px solid #535b66;
  border-radius: 12px; padding: 16px; box-sizing: border-box;
  font: 14px/1.6 -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
}
.academy-panel .widget-html, .academy-panel .widget-html-content {
  width: 100%; min-width: 0; box-sizing: border-box;
}
.academy-panel .widget-html-content {
  color: #eeeeee; white-space: normal; overflow-wrap: anywhere;
  line-height: 1.6; font-size: 14px;
}
.academy-panel p { margin: 8px 0; }
.academy-panel svg { display: block; width: 100%; height: auto; }
.academy-panel .academy-title { font-size: 17px; font-weight: 650; }
.academy-panel .academy-note { color: #b8c2cc; margin: 6px 0 12px; }
.academy-panel a { color: #8cc8ff; text-decoration: underline; }
.academy-panel hr { border: 0; border-top: 1px solid #535b66; margin: 12px 0; }
.academy-panel .widget-label { color: #eeeeee; }
.academy-panel .jupyter-button {
  background: #30343b; color: #eeeeee; border: 1px solid #798491;
  border-radius: 6px; height: 38px; font-size: 14px;
}
.academy-panel .jupyter-button:hover { background: #414955; }
.academy-panel .jupyter-button:focus-visible,
.academy-panel select:focus-visible { outline: 2px solid #8cc8ff; outline-offset: 2px; }
.academy-panel .jupyter-button.mod-warning {
  background: #57431c; color: #ffe09a; border-color: #e8bb55;
}
.academy-panel .jupyter-button.mod-info {
  background: #164567; color: #e4f3ff; border-color: #8cc8ff;
}
.academy-panel .jupyter-button:disabled { opacity: .65; }
.academy-panel select, .academy-panel input {
  background: #141414; color: #eeeeee; border-color: #798491;
}
.academy-panel .widget-dropdown { height: auto; min-height: 40px; align-items: center; }
.academy-panel .widget-dropdown select {
  appearance: none; -webkit-appearance: none; color-scheme: dark;
  width: 100%; min-width: 0; min-height: 38px; box-sizing: border-box;
  font-size: 14px; line-height: 1.4; padding: 7px 48px 7px 12px;
  border: 1px solid #798491; border-radius: 6px; cursor: pointer;
  background-color: #141414;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 12'%3E%3Cpath d='M3 3l5 5 5-5' fill='none' stroke='%23eeeeee' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'/%3E%3C/svg%3E"),
    linear-gradient(to left, #30343b 0, #30343b 39px, #798491 39px, #798491 40px, transparent 40px);
  background-position: right 12px center, right center;
  background-size: 16px 12px, 100% 100%; background-repeat: no-repeat;
}
.academy-panel .widget-dropdown select:hover { border-color: #8cc8ff; }
.academy-panel .widget-readout { color: #eeeeee; }
.academy-panel .noUi-target { background: #555e6b; border-color: #798491; }
.academy-panel .noUi-connect { background: #65d6ae; }
.academy-panel .noUi-handle { background: #cdd6df; border-color: #eeeeee; }
.academy-panel .academy-check {
  background: #141414; border-left: 4px solid; border-radius: 4px;
  padding: 8px 12px; margin: 6px 0; overflow-wrap: anywhere;
}
.academy-panel .academy-controls { gap: 8px; }
</style>'''


def html(content=''):
    return widgets.HTML(content, layout=widgets.Layout(width='100%', min_width='0'))


def panel(children):
    view = widgets.VBox([html(STYLE), *children],
                        layout=widgets.Layout(width='100%', max_width='700px', min_width='0'))
    view.add_class('academy-panel')
    return view


def controls(children):
    row = widgets.Box(children, layout=widgets.Layout(
        display='flex', flex_flow='row wrap', width='100%', min_width='0',
        margin='6px 0'))
    row.add_class('academy-controls')
    return row


def check_card(content, color):
    return f'<div class="academy-check" style="border-color:{color}">{content}</div>'
