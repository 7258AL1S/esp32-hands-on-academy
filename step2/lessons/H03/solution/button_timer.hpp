#pragma once
#include <cstdint>

namespace desk {

    enum class Event {
        none,
        started,
        cancelled,
        finished
    };

    // Reference policy only: raw GPIO mapping and real time live outside this class.
    class ButtonTimer {
    public:
        Event update(bool pressed, uint64_t now_ms) {
            if (!initialized_) {
                initialized_ = true;
                stable_ = candidate_ = pressed;
                since_ = now_ms;
                armed_ = !pressed;
            }
            if (pressed != candidate_) {
                candidate_ = pressed;
                since_ = now_ms;
            }
            if (candidate_ != stable_ && now_ms - since_ >= 30) {
                stable_ = candidate_;
                if (!stable_) {
                    armed_ = true;
                } else if (armed_) {
                    armed_ = false;
                    if (running_) {
                        running_ = false;
                        return Event::cancelled; // Cancellation wins at the deadline.
                    }
                    running_ = true;
                    deadline_ = now_ms + 5000;
                    return Event::started;
                }
            }
            if (running_ && now_ms >= deadline_) {
                running_ = false;
                return Event::finished;
            }
            return Event::none;
        }
        bool running() const {
            return running_;
        }

    private:
        bool initialized_ = false;
        bool candidate_ = false;
        bool stable_ = false;
        bool armed_ = false;
        bool running_ = false;
        uint64_t since_ = 0;
        uint64_t deadline_ = 0;
    };

} // namespace desk
