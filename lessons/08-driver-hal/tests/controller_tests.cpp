#include "academy/device.hpp"
#include "academy/test.hpp"
#include "controller.hpp"
#include <string>
using namespace academy;int main(int argc,char**argv){std::string p=argc==2?argv[1]:"exercise";TestSuite t;if(p=="guided"){t.run("calibrated sensor value",[]{Device d;Controller c(d);d.input.temperature=22;c.tick();near(d.output.reading,22.5,.01,"offset");});return t.result();}t.run("connected sensor reports calibrated value",[]{Device d;Controller c(d);d.input.temperature=27.5;c.tick();near(d.output.reading,28,.01,"calibration");require(d.output.led,"alert");});t.run("disconnect is error not stale value",[]{Device d;Controller c(d);d.input.temperature=30;c.tick();d.input.connected=false;c.tick();require(d.output.error&&!d.output.led&&d.output.state=="sensor error","disconnect");});t.run("fault is error",[]{Device d;Controller c(d);d.input.fault=true;c.tick();require(d.output.error,"fault");});return t.result();}
