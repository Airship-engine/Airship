#include "spdlog/spdlog.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>

namespace Airship::Logging {
    std::shared_ptr<spdlog::logger> g_ApplicationLogger;

    void Init()
    {
        spdlog::enable_backtrace(32);

        g_ApplicationLogger = spdlog::stdout_color_mt("console");    
    }
}
