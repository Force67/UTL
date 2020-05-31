
/*
 * UTL : The universal utility library
 *
 * Copyright 2019-2020 Force67.
 * For information regarding licensing see LICENSE
 * in the root of the source tree.
 */

#include <mutex>
#include <thread>
#include <vector>

#include "logger.h"

namespace utl {

const char* GetLevelName(logLevel log_level) {
#define LVL(x)                                                                                     \
    case logLevel::x:                                                                              \
        return #x
    switch (log_level) {
        LVL(Trace);
        LVL(Debug);
        LVL(Info);
        LVL(Warning);
        LVL(Error);
        LVL(Critical);
        // case LogLevel::Count:
        // UNREACHABLE();
    }
#undef LVL
    return nullptr;
}

// init platform specific logging facilities
void logPlatformInit(logRegistry&, bool);

static logRegistry* g_logRegistry = nullptr;

logRegistry* getLogRegistry() {
    if (!g_logRegistry)
        BUGCHECK();

    return g_logRegistry;
}

logRegistry::logRegistry() {
    g_logRegistry = this;
    timestamp = std::chrono::steady_clock::now();

    backendThread = std::thread([&] {
        logEntry entry;
        auto write_logs = [&](logEntry& e) {
            std::lock_guard lock{writeLock};
            for (const auto& sink : sinks) {
                sink->write(e);
            }
        };

        // work the queue
        while (true) {
            entry = pending.PopWait();
            if (requestExit) return;

            write_logs(entry);
        }

        // drain the queue
        // only writes up to MAX_LOGS
        constexpr int MAX_LOGS_TO_WRITE = 100;
        int logs_written = 0;
        while (logs_written++ < MAX_LOGS_TO_WRITE && pending.Pop(entry)) {
            write_logs(entry);
        }
    });
}

logRegistry::~logRegistry() {
    g_logRegistry = nullptr;
    requestExit = true;

    // flush
    logEntry entry;
    pending.Push(entry);
    backendThread.join();
}

void logRegistry::addEntry(logLevel lvl, u32 line, const char* func, std::string msg) {
    using std::chrono::duration_cast;
    using std::chrono::steady_clock;

    logEntry entry{};
    entry.timestamp = duration_cast<std::chrono::microseconds>(steady_clock::now() - timestamp);
    entry.log_level = lvl;
    entry.line_num = line;
    entry.function = func;
    entry.message = std::move(msg);

    pending.Push(entry);
}

logBase* logRegistry::addSink(logBase* sink) {
    std::lock_guard lock{writeLock};
    sinks.push_back(sink);
    return sink;
}

void logRegistry::removeSink(std::string_view name) {
    std::lock_guard lock{writeLock};
    const auto it = std::remove_if(sinks.begin(), sinks.end(),
                                   [&name](const auto& i) { return name == i->getName(); });
    sinks.erase(it, sinks.end());
}

void logRegistry::removeSink(logBase* sink) {
    std::lock_guard lock{writeLock};
    const auto it = std::remove_if(sinks.begin(), sinks.end(),
                                   [&sink](const auto& i) { return sink == i; });
    sinks.erase(it, sinks.end());
}

logBase* logRegistry::getSink(std::string_view name) {
    const auto it = std::find_if(sinks.begin(), sinks.end(),
                                 [&name](const auto& i) { return name == i->getName(); });
    if (it == sinks.end())
        return nullptr;
    return *it;
}

UniquePtr<logRegistry> createLogger(bool withc) {
    auto lr = std::make_unique<logRegistry>();
    logPlatformInit(*lr, withc);
    return std::move(lr);
}

void formatLogMsg(logLevel lvl, uint32_t line, const char* func, const char* fmt,
                  const fmt::format_args& args) {
    if (g_logRegistry)
        g_logRegistry->addEntry(lvl, line, func, fmt::vformat(fmt, args));
}

std::string formatLogEntry(const logEntry& entry) {
    u32 time_seconds = static_cast<unsigned int>(entry.timestamp.count() / 1000000);
    u32 time_fractional = static_cast<unsigned int>(entry.timestamp.count() % 1000000);

    const char* level_name = GetLevelName(entry.log_level);

    return fmt::format("[{:4d}.{:06d}] <{}> {}:{}: {}", time_seconds, time_fractional, level_name,
                       entry.function, entry.line_num, entry.message);
}
}