#pragma once

// UTL: The universal utility library
// Copyright 2019-2020 Force67.

#include <cstdint>

namespace utl {

class init_function {
public:
    using callback_t = void (*)();

private:
    init_function* next_{nullptr};
    callback_t callback_{nullptr};

    init_function(init_function*& parent, callback_t callback) noexcept;

public:
    init_function(callback_t callback) noexcept;
    init_function(init_function& parent, callback_t callback) noexcept;

    init_function(const init_function&) = delete;
    init_function(init_function&&) = delete;

    static init_function*& ROOT() noexcept;

    static size_t init();
};

inline init_function::init_function(init_function*& parent, callback_t callback) noexcept
    : next_(parent),
      callback_(callback) {
    parent = this;
}

inline init_function::init_function(callback_t callback) noexcept
    : init_function(ROOT(), callback) {}

inline init_function::init_function(init_function& parent, callback_t callback) noexcept
    : init_function(parent.next_, callback) {}

inline init_function*& init_function::ROOT() noexcept {
    static init_function* root{nullptr};

    return root;
}

inline size_t init_function::init() {
    size_t total = 0;

    for (init_function* i = ROOT(); i;) {
        if (i->callback_) {
            i->callback_();
            i->callback_ = nullptr;

            ++total;
        }

        init_function* j = i->next_;
        i->next_ = nullptr;
        i = j;
    }

    ROOT() = nullptr;

    return total;
}
} // namespace mem