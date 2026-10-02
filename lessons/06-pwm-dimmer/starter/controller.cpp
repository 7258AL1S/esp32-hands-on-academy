#include "controller.hpp"
namespace academy {Controller::Controller(Device&b):board_(b){}void Controller::tick(){board_.output.brightness=board_.input.analog/41;board_.output.led=board_.output.brightness>0;}}
