#pragma once

#if defined(NDEBUG)
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/callback_sink.h"
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
	enum class Level
	{
        DEBUG,
        INFO,
        ALERT,
        ERROR,
        MAYDAY
	};

	static ShipLog& get()
	{
		static ShipLog log;
		return log;
	}

	ShipLog()
	{
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consoleSink->set_level(spdlog::level::info);
		m_ActiveSinks.emplace("default_log", consoleSink);

		m_Logger = std::make_shared<spdlog::logger>("airship", consoleSink);
		m_Logger->enable_backtrace(32);
		m_Logger->set_pattern("[%l] %^%T.%e %s:%# [%!]: %v%$");
		m_Logger->set_level(spdlog::level::trace);
		m_Logger->flush_on(spdlog::level::err);

		spdlog::register_logger(m_Logger);
	}

	spdlog::logger* GetLogger()
	{
		return m_Logger.get();
	}

	bool AddFileOutput(const std::string& name, const std::string& filename, Level level)
	{
		if (m_ActiveSinks.contains(name))
			return false;

		auto [it, sinkAdded] = m_ActiveSinks.emplace(name, std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename.c_str(), true));
		if (!sinkAdded)
			return false;

		std::shared_ptr<spdlog::sinks::sink> newSink = it->second;
		newSink->set_level(ToSpdLog(level));
		m_Logger->sinks().push_back(std::move(newSink));
		return true;
	}

	bool AddListener(const std::string& name, std::function<void(Level level, const std::string_view)> listener, Level level)
	{
		if (m_ActiveSinks.contains(name))
			return false;

		auto [it, sinkAdded] = m_ActiveSinks.emplace(name, std::make_shared<spdlog::sinks::callback_sink_mt>([listener](const spdlog::details::log_msg &msg)
			{ 
				listener(FromSpdLog(msg.level), msg.payload);
			}));
		if (!sinkAdded)
			return false;

		std::shared_ptr<spdlog::sinks::sink> newSink = it->second;
		newSink->set_level(ToSpdLog(level));
		m_Logger->sinks().push_back(std::move(newSink));
		return true;
	}

	bool RemoveOutput(const std::string &name)
	{
		if (!m_ActiveSinks.contains(name))
			return false;

		auto it = m_ActiveSinks.find(name);
		auto loggerIt = std::find(m_Logger->sinks().begin(), m_Logger->sinks().end(), it->second);
		m_Logger->sinks().erase(loggerIt);

		m_ActiveSinks.erase(it);
		return true;
	}

	bool SetLevel(const std::string &name, Level level)
	{
		if (!m_ActiveSinks.contains(name))
			return false;

		m_ActiveSinks.at(name)->set_level(ToSpdLog(level));
		return true;
	}

	void FlushLogs()
	{
		m_Logger->flush();
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

	constexpr static Level FromSpdLog(spdlog::level::level_enum level)
	{
		switch (level)
		{
		case spdlog::level::trace:
			[[fallthrough]];
		case spdlog::level::debug:
			return Level::DEBUG;
		case spdlog::level::info:
			return Level::INFO;
		case spdlog::level::warn:
			return Level::ALERT;
		case spdlog::level::err:
			return Level::ERROR;
		case spdlog::level::critical:
			return Level::MAYDAY;
		default:
			break;
		}
		assert("Invalid log level");
		return Level::INFO;
	}

	std::shared_ptr<spdlog::logger> m_Logger;
	std::unordered_map<std::string, spdlog::sink_ptr> m_ActiveSinks;
};

} // namespace Airship
