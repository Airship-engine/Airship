#pragma once

#if defined(NDEBUG)
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include <map>
#include <memory>
#include <string>

#define SHIPLOG_DEBUG( ... ) SPDLOG_LOGGER_DEBUG(Airship::ShipLog::get().GetLogger(), __VA_ARGS__)
#define SHIPLOG_INFO( ... ) SPDLOG_LOGGER_INFO(Airship::ShipLog::get().GetLogger(), __VA_ARGS__)
#define SHIPLOG_ALERT( ... ) SPDLOG_LOGGER_WARN(Airship::ShipLog::get().GetLogger(), __VA_ARGS__)
#define SHIPLOG_ERROR( ... ) SPDLOG_LOGGER_ERROR(Airship::ShipLog::get().GetLogger(), __VA_ARGS__)
#define SHIPLOG_MAYDAY( ... ) SPDLOG_LOGGER_CRITICAL(Airship::ShipLog::get().GetLogger(), __VA_ARGS__)

namespace Airship
{

class ShipLog 
{
public: 
    enum class Level {
        DEBUG,
        INFO,
        ALERT,
        ERROR,
        MAYDAY
    };

    static ShipLog& get() {
        static ShipLog log;
        return log;
    }

    ShipLog() 
    {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::info);

        logger = std::make_shared<spdlog::logger>("airship", consoleSink);
        logger->enable_backtrace(32);
        logger->set_pattern("[%l] %^%T.%e %s:%# [%!]: %v%$");
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::err);

        spdlog::register_logger(logger);
    }

    spdlog::logger* GetLogger() 
    {
        return logger.get();
    }

    bool AddFileOutput(const std::string& name, const std::string& filename, Level level) 
    {
        if (activeSinks.contains(name))
            return false;

        auto [it, sinkAdded] = activeSinks.emplace(name, std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename.c_str(), true));
        if (!sinkAdded)
            return false;

        std::shared_ptr<spdlog::sinks::sink> newSink = it->second;
        newSink->set_level(ToSpdLog(level));
        logger->sinks().push_back(std::move(newSink));
        return true;
    }

    bool RemoveOutput(const std::string& name)
    {
        if(!activeSinks.contains(name))
            return false;

        auto it = activeSinks.find(name);
        auto loggerIt = std::find(logger->sinks().begin(), logger->sinks().end(), it->second);
        logger->sinks().erase(loggerIt);

        activeSinks.erase(it);
        return true;
    }

    bool SetLevel(const std::string& name, Level level) 
    {
        if (!activeSinks.contains(name))
            return false;
        
        activeSinks.at(name)->set_level(ToSpdLog(level));
        return true;
    }

    void FlushLogs()
    {
        logger->flush();
    }

    constexpr static bool IsLevelEnabled(Level level)
    {
        return ToSpdLog(level) >= SPDLOG_ACTIVE_LEVEL;
    }

private:
    constexpr static spdlog::level::level_enum ToSpdLog(Level shipLogLevel)
    {
        switch (shipLogLevel) 
        {
            case Level::DEBUG: 
                return spdlog::level::debug;
            case Level::INFO: 
                return spdlog::level::info;
            case Level::ALERT: 
                return spdlog::level::warn;
            case Level::ERROR: 
                return spdlog::level::err;
            case Level::MAYDAY: 
                return spdlog::level::critical;
        }
        assert(false);
        return spdlog::level::info;
    }

    std::shared_ptr<spdlog::logger> logger;
    std::unordered_map<std::string, spdlog::sink_ptr> activeSinks;
};

} // namespace Airship
