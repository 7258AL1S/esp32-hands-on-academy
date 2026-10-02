#include "academy/device.hpp"
#include "academy/test.hpp"
#include "controller.hpp"
#include <string>
using namespace academy;int main(int argc,char**argv){std::string p=argc==2?argv[1]:"exercise";TestSuite t;if(p=="guided"){t.run("midpoint maps to voltage",[]{Device d;Controller c(d);d.input.analog=2048;c.tick();near(d.output.reading,1.65,.01,"midpoint");});return t.result();}t.run("ADC endpoints and threshold",[]{Device d;Controller c(d);d.input.analog=0;c.tick();near(d.output.reading,0,.001,"zero");d.input.analog=4095;c.tick();near(d.output.reading,3.3,.001,"full scale");require(d.output.led,"threshold");});t.run("fault does not look like measurement",[]{Device d;Controller c(d);d.input.analog=4095;d.input.fault=true;c.tick();require(d.output.error&&!d.output.led,"fault indication");});return t.result();}
