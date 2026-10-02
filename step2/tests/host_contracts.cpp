#include H03_POLICY_HEADER
#include H05_PARSER_HEADER
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

static void require(bool condition, const char* description) {
    if (!condition) {
        std::cerr << "FAIL: " << description << '\n';
        std::exit(1);
    }
}
static void collect(const char* bytes, size_t size, void* context) {
    static_cast<std::vector<std::string>*>(context)->emplace_back(bytes, size);
}
int main() {
    using desk::Event;
    desk::ButtonTimer startup;
    require(startup.update(true, 0) == Event::none, "startup held never starts");
    require(startup.update(true, 100) == Event::none, "hold does not arm");
    startup.update(false, 101);
    startup.update(false, 131);
    startup.update(true, 140);
    require(startup.update(true, 169) == Event::none, "29ms unstable");
    require(startup.update(true, 170) == Event::started, "30ms stable starts");
    require(startup.update(true, 200) == Event::none, "held no duplicate");
    require(startup.update(true, 5170) == Event::finished, "deadline finishes once");
    require(startup.update(true, 6000) == Event::none, "finished no duplicate");
    desk::ButtonTimer cancelling;
    cancelling.update(false, 0);
    cancelling.update(true, 1);
    cancelling.update(false, 10);
    cancelling.update(true, 20);
    require(cancelling.update(true, 49) == Event::none,
            "bounce restarts stable window");
    require(cancelling.update(true, 50) == Event::started, "stable after bounce");
    cancelling.update(false, 70);
    cancelling.update(false, 100);
    cancelling.update(true, 5020);
    require(cancelling.update(true, 5050) == Event::cancelled,
            "cancel wins exact deadline");
    std::vector<std::string> frames;
    desk::LineParser parser(collect, &frames);
    for (char byte : std::string("STA"))
        parser.feed(byte, 0);
    require(frames.empty(), "partial frame is not delivered");
    for (char byte : std::string("TUS\nA\nB\n"))
        parser.feed(byte, 50);
    require(frames == std::vector<std::string>({"STATUS", "A", "B"}),
            "split and glued frames");
    for (char byte : std::string(17, 'x') + "\nOK\n")
        parser.feed(byte, 100);
    require(frames.back() == "OK" && frames.size() == 4,
            "overflow discarded, next frame recovered");
    parser.feed('X', 200);
    parser.expire(1200);
    for (char byte : std::string("NEW\n"))
        parser.feed(byte, 1201);
    require(frames.back() == "NEW", "timeout discards previous partial bytes");
    for (char byte : std::string(16, 'a') + "\n")
        parser.feed(byte, 1300);
    require(frames.back() == std::string(16, 'a'), "exact capacity frame valid");
    std::cout << "PASS: startup, debounce boundary, hold, deadline/cancel; UART "
                 "fragmentation/overflow/timeout\n";
}
