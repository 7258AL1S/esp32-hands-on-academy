#include "controller.hpp"
namespace academy { Controller::Controller(Device& b):board_(b){} void Controller::tick(){bool p=board_.read_button()==Level::low; if(!initialized_){initialized_=true;last_=p;return;} if(p&&!last_){running_=!running_;++board_.output.events;} last_=p; board_.output.led=running_; board_.output.state=running_?"running":"idle";} }
