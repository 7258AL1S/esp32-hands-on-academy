#include "controller.hpp"
namespace academy {Controller::Controller(Device&b):board_(b){}void Controller::tick(){board_.output.reading=board_.input.temperature+.5;board_.output.led=board_.output.reading>=28.;}}
