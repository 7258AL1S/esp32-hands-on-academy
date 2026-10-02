#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace desk {

    // callback consumes the bytes synchronously; pointers are invalid after next feed.
    class LineParser {
    public:
        using Callback = void (*)(const char*, size_t, void*);
        explicit LineParser(Callback callback, void* context)
            : callback_(callback), context_(context) {
        }

        void feed(char byte, uint64_t now_ms) {
            expire(now_ms);
            active_ = true;
            last_ms_ = now_ms;
            if (byte == '\n') {
                if (!discarding_ && used_ && callback_) {
                    callback_(buffer_.data(), used_, context_);
                }
                reset();
            } else if (!discarding_) {
                if (used_ == buffer_.size()) {
                    discarding_ = true;
                    used_ = 0;
                } else {
                    buffer_[used_++] = byte;
                }
            }
        }
        void expire(uint64_t now_ms) {
            if (active_ && now_ms - last_ms_ >= 1000) {
                reset();
            }
        }

    private:
        void reset() {
            used_ = 0;
            discarding_ = false;
            active_ = false;
        }
        std::array<char, 16> buffer_{};
        size_t used_ = 0;
        bool discarding_ = false;
        bool active_ = false;
        uint64_t last_ms_ = 0;
        Callback callback_;
        void* context_;
    };

} // namespace desk
