#include "academy/device.hpp"
#include "academy/test.hpp"
#include "controller.hpp"
#include <string>
using namespace academy;static void send(Device&d,Controller&c,const char*s){d.input.text=s;c.tick();d.input.text="";c.tick();}
int main(int argc,char**argv){std::string p=argc==2?argv[1]:"exercise";TestSuite t;if(p=="guided"){t.run("PING gets PONG",[]{Device d;Controller c(d);send(d,c,"PING\n");require(d.output.outgoing=="PONG\n","response");});return t.result();}t.run("partial frame waits for newline",[]{Device d;Controller c(d);send(d,c,"PI");require(d.output.outgoing.empty(),"no incomplete response");send(d,c,"NG\n");require(d.output.outgoing=="PONG\n","joined frame");});t.run("multiple commands and LED",[]{Device d;Controller c(d);send(d,c,"LED:1\nLED:0\n");require(!d.output.led&&d.output.outgoing=="OK\nOK\n","two frames");});t.run("disconnect is visible",[]{Device d;Controller c(d);d.input.connected=false;send(d,c,"PING\n");require(d.output.error,"disconnect");});return t.result();}
