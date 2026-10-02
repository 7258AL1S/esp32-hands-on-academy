#include "status_policy.hpp"
namespace status_policy {
    bool should_show_ready(bool connected, bool fault) {
        return connected && !fault;
    }
}
