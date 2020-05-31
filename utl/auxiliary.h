#pragma once

/*
 * UTL : The universal utility library
 *
 * Copyright 2019-2020 Force67.
 * For information regarding licensing see LICENSE
 * in the root of the source tree.
 */

// platform agnostic helpers for enhancing platform experience

#include "utypes.h"
#include <string>

namespace utl {

void openBrowserLink(const std::string &url /*TODO: allow string_view*/);
void setDPIAware();

void getOptimalWindowSize(utl::i32&, utl::i32&);
}