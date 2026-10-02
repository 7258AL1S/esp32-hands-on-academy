#pragma once
#include <cmath>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
namespace academy {
inline void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}
inline void near(double actual, double expected, double tolerance, const std::string& message) {
    require(std::isfinite(actual) && std::abs(actual - expected) <= tolerance,
            message + ": expected=" + std::to_string(expected) + ", actual=" + std::to_string(actual));
}
class TestSuite {
    int passed_ = 0, failed_ = 0;
public:
    void run(const std::string& title, const std::function<void()>& fn) {
        try { fn(); ++passed_; std::cout << "[PASS] " << title << '\n'; }
        catch (const std::exception& e) { ++failed_; std::cout << "[FAIL] " << title << "\n       " << e.what() << '\n'; }
    }
    int result() const {
        std::cout << passed_ << '/' << passed_ + failed_ << " checks passed\n";
        return failed_ ? 1 : 0;
    }
};
} // namespace academy
