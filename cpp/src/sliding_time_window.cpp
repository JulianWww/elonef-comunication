#include <elonef-communication/sliding_time_window.hpp>
#include <elonef-communication/encoding/message.hpp>

Elonef::SlidingTimeWindow::SlidingTimeWindow(size_t _upper_limit, size_t _lower_limit) : lower_limit(_lower_limit), upper_limit(_upper_limit) {}

bool Elonef::SlidingTimeWindow::in_window(size_t value) {
    size_t center = get_current_time();
    return center - lower_limit < value && center + upper_limit > value;
}