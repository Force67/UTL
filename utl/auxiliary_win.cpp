
/*
 * UTL : The universal utility library
 *
 * Copyright 2019-2020 Force67.
 * For information regarding licensing see LICENSE
 * in the root of the source tree.
 */

#include <Windows.h>
#include <shellapi.h> // for ShellExecuteA
#include <shellscalingapi.h>
#include <VersionHelpers.h>

#include "auxiliary.h"
#include "string_util.h"

namespace utl {

void openBrowserLink(const std::string& url) {
    auto wide = utf8_to_utf16_w(url);
    ShellExecuteW(nullptr, L"open", reinterpret_cast<LPCWSTR>(wide.c_str()), nullptr, nullptr,
                  SW_SHOWNORMAL);
}

void setDPIAware() {
    // windows 10 exclusive scaling api
    auto hUser32 = ::LoadLibraryW(L"user32.dll");
    if (auto* pfunc = (decltype(&::SetThreadDpiAwarenessContext))::GetProcAddress(
            hUser32, "SetThreadDpiAwarenessContext")) {

        pfunc(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        return;
    }

    // w8 scaling
    if (IsWindows8Point1OrGreater()) {
        auto hShcore = ::LoadLibraryW(L"shcore.dll");
        if (auto* pfunc = (decltype(&::SetProcessDpiAwareness))::GetProcAddress(
                hShcore, "SetProcessDpiAwareness")) {

            pfunc(PROCESS_PER_MONITOR_DPI_AWARE);
            return;
        }
    }

    // legacy scaling
    SetProcessDPIAware();
}

void getOptimalWindowSize(utl::i32 &x, utl::i32 &y) {
  x = GetSystemMetrics(0);
  y = GetSystemMetrics(1);
}
}