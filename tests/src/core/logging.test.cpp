#include "core/logging.h"
#include "gtest/gtest.h"

#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>

// Temporary filename creation has no platform-agnostic API
#ifdef _WIN32
// Windows leaks a define for ERROR that clashes with log levels. We might need to
// rename our log levels as lowercase if we need windows.h elsewhere. This is a workaround.

// NOLINTBEGIN(misc-include-cleaner)
#define NOGDI
#define NOMINMAX
#include <windows.h>
#include <array>
namespace {
    std::string create_temp_file() {
        std::array<char, MAX_PATH+1> temp_path{};
        DWORD path_len = GetTempPathA(temp_path.size(), temp_path.data());
        if (path_len == 0 || path_len > MAX_PATH + 1) {
            throw std::runtime_error("Error getting temporary path.");
        }
    
        std::array<char, MAX_PATH+1> temp_filename{};
        UINT unique_int = GetTempFileNameA(temp_path.data(), "tmp", 0, temp_filename.data());
        if (unique_int == 0) {
             throw std::runtime_error("Error getting temporary file name.");
        }
        return std::string(temp_filename.data());
    }
} // anonymous namespace
// NOLINTEND(misc-include-cleaner)
#else
#include <unistd.h>
#include <cerrno>
#include <stdlib.h> // NOLINT // mkstemp only guaranteed in stdlib.h on POSIX
#include <cstring>
namespace {
    std::string create_temp_file() {
        std::string template_ = "/tmp/tempfile.XXXXXX";
        const int fd = mkstemp(template_.data());
        if (fd == -1) {
            throw std::runtime_error("Error creating temporary file: " + std::string(strerror(errno)));
        }
        // Close the file. This may leak empty files if the filename isn't used,
        // but they're empty so this is acceptable for now.
        close(fd);
        return template_;
    }
}
#endif

TEST(Logging, coutinfo)
{
    SHIPLOG_DEBUG("This is a test macro debug log");
    SHIPLOG_DEBUG("This is a test macro debug log {}", "Now with formatting!");
    SHIPLOG_INFO("This is a test macro info log");
    SHIPLOG_INFO("This is a test macro info log {}", "Now with formatting!");
    SHIPLOG_ALERT("This is a test macro alert log");
    SHIPLOG_ALERT("This is a test macro alert log {}", "Now with formatting!");
    SHIPLOG_ERROR("This is a test macro error log");
    SHIPLOG_ERROR("This is a test macro error log {}", "Now with formatting!");
    SHIPLOG_MAYDAY("This is a test macro mayday log");
    SHIPLOG_MAYDAY("This is a test macro mayday log {}", "Now with formatting!");
}

TEST(Logging, fileoutput)
{
    const std::string filename = create_temp_file();
    SHIPLOG_ERROR(filename);

    Airship::ShipLog::get().AddFileOutput("test log", filename, Airship::ShipLog::Level::ERROR);
    SHIPLOG_ERROR("Logging an error");
    Airship::ShipLog::get().FlushLogs();
    Airship::ShipLog::get().RemoveOutput("test log");
    ASSERT_TRUE(std::filesystem::exists(filename.c_str()));

    {
        const std::ifstream tmpFile(filename); // this is equivalent to the above method
        if(!tmpFile.good())
        {
            FAIL() << "Failed to open the tmpfile: " << filename;
        }
        std::stringstream fileContents;
        fileContents << tmpFile.rdbuf();
        if constexpr (Airship::ShipLog::IsLevelEnabled(Airship::ShipLog::Level::ERROR))
            ASSERT_NE(fileContents.str().find("Logging an error"), std::string::npos) << "File: " << filename << "\nFile Contents: \"" << fileContents.str() << "\"";
        else
            ASSERT_EQ(fileContents.str().find("Logging an error"), std::string::npos) << "File: " << filename << "\nFile Contents: \"" << fileContents.str() << "\"";
    }

    Airship::ShipLog::get().AddFileOutput("test log", filename, Airship::ShipLog::Level::ERROR);
    SHIPLOG_DEBUG("This is a debug message");
    Airship::ShipLog::get().FlushLogs();
    Airship::ShipLog::get().RemoveOutput("test log");

    {
        const std::ifstream tmpFile(filename); // this is equivalent to the above method
        if(!tmpFile.good())
        {
            FAIL() << "Failed to open the tmpfile: " << filename;
        }
        std::stringstream fileContents;
        fileContents << tmpFile.rdbuf();
        // The file is set to receive error and mayday messages only.
        ASSERT_EQ(fileContents.str().find("This is a debug message"), std::string::npos) << "File: " << filename << "\nFile Contents: \"" << fileContents.str() << "\"";
    }

    std::filesystem::remove(filename.c_str());
    ASSERT_FALSE(std::filesystem::exists(filename.c_str()));
}
