#pragma once

// UTL: The universal utility library
// Copyright 2019-2020 Force67.

#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "base.h"
#include "threadsafe_queue.h"

// hard dependency
#include <fmt/format.h>

namespace utl {

enum class logLevel : u8 {
    Trace,    //< very detailed and extensive debugging information
    Debug,    //< less detailed debug information
    Info,     //< status information
    Warning,  //< minor or potential problems
    Error,    //< major problems that block further execution of a task
    Critical, //< major problems that block continued execution of everything
    Count     //< total number of log levels
};

struct logEntry {
    std::chrono::microseconds timestamp;
    logLevel log_level;
    u32 line_num;
    std::string function;
    std::string message;

    logEntry() = default;
    logEntry(logEntry&& o) = default;

    logEntry& operator=(logEntry&& o) = default;
    logEntry& operator=(const logEntry& o) = default;
};

class logBase {
public:
    virtual ~logBase() = default;

    virtual const char* getName() { return "logBase"; }
    virtual void write(const logEntry&) = 0;
};

class logRegistry {
public:
    logRegistry();
    ~logRegistry();

    inline void addEntry(logLevel, u32, const char*, std::string);

    logBase* addSink(logBase* sink);
    void removeSink(logBase* sink);

    void removeSink(std::string_view name);
    logBase* getSink(std::string_view name);

private:
    bool requestExit = false;
    std::mutex writeLock;
    std::thread backendThread;
    std::vector<logBase*> sinks;
    MPSCQueue<logEntry> pending;
    std::chrono::steady_clock::time_point timestamp;
};

// user has to manage lifetime
UniquePtr<logRegistry> createLogger(bool withConsole = false);

// instance
logRegistry* getLogRegistry();

// default log formatting
std::string formatLogEntry(const logEntry&);
const char* GetLevelName(logLevel);

void formatLogMsg(logLevel, u32, const char*, const char*, const fmt::format_args&);

template <typename... Args>
inline void fmtLogMsg(logLevel lvl, u32 line, const char* func, const char* fmt,
                      const Args&... args) {
    formatLogMsg(lvl, line, func, fmt, fmt::make_format_args(args...));
}

template <typename... Args>
inline void fmtLogMsg(logLevel lvl, u32 line, const char* func, const std::string& text) {
    formatLogMsg(lvl, line, func, text.c_str(), {});
}
}

#ifndef SHIPPING
#define LOG_TRACE(...) ::utl::fmtLogMsg(::utl::logLevel::Trace, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_TRACE(fmt, ...) (void(0))
#endif

#define LOG_DEBUG(...) ::utl::fmtLogMsg(::utl::logLevel::Debug, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...) ::utl::fmtLogMsg(::utl::logLevel::Info, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARNING(...) ::utl::fmtLogMsg(::utl::logLevel::Warning, __LINE__, __func__, __VA_ARGS__)
#define LOG_ERROR(...) ::utl::fmtLogMsg(::utl::logLevel::Error, __LINE__, __func__, __VA_ARGS__)
#define LOG_CRITICAL(...) ::utl::fmtLogMsg(::utl::logLevel::Critical, __LINE__, __func__, __VA_ARGS__)

#define LOG_ASSERT(expression)                                                                     \
    do {                                                                                           \
        if (!(expression)) {                                                                       \
            ::utl::fmtLogMsg(::utl::logLevel::Error, __LINE__, __func__,                           \
                             "assertion failed at " #expression);                                  \
            __debugbreak();                                                                        \
        }                                                                                          \
    \
} while (0)

#define LOG_UNIMPLEMENTED                                                                          \
    ::utl::fmtLogMsg(::utl::logLevel::Error, __LINE__, __func__, "Unimplemented function")
