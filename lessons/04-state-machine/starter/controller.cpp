#include "controller.hpp"
namespace academy {Controller::Controller(Device&b):board_(b){} void Controller::tick(){if(board_.input.text!="start"||board_.input.text==last_command_){board_.output.state=state_;return;}last_command_=board_.input.text;state_="running";board_.output.state=state_;board_.output.led=true;++board_.output.events;}}
