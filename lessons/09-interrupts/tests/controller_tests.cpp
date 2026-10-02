#include "academy/device.hpp"
#include "academy/test.hpp"
#include "controller.hpp"
#include <string>
using namespace academy;int main(int argc,char**argv){std::string p=argc==2?argv[1]:"exercise";TestSuite t;if(p=="guided"){t.run("visible count follows pulses",[]{Device d;Controller c(d);d.input.pulses=2;c.tick();require(d.output.events==2,"count");});return t.result();}t.run("burst is consumed without loss",[]{Device d;Controller c(d);d.input.pulses=0;c.tick();d.input.pulses=3;c.tick();require(d.output.events==3,"all three pulses");d.input.pulses=5;c.tick();require(d.output.events==5,"next burst");});t.run("boot count is baseline",[]{Device d;d.input.pulses=8;Controller c(d);c.tick();require(d.output.events==0,"startup baseline");});t.run("counter rollback is fault",[]{Device d;Controller c(d);c.tick();d.input.pulses=4;c.tick();d.input.pulses=1;c.tick();require(d.output.error,"rollback");});return t.result();}
