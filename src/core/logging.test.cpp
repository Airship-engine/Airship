#include "core/logging.h"
#include "gtest/gtest.h"

#include <cstdio>
#include <fstream>
#include <filesystem>

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
    std::string filename = std::tmpnam(nullptr);
    SHIPLOG_ERROR(filename);

    Airship::ShipLog::get().AddFileOutput("test log", filename, Airship::ShipLog::Level::ERROR);
    SHIPLOG_ERROR("Logging an error");
    Airship::ShipLog::get().FlushLogs();
    Airship::ShipLog::get().RemoveOutput("test log");
    ASSERT_TRUE(std::filesystem::exists(filename.c_str()));

    {
        std::ifstream tmpFile(filename); // this is equivalent to the above method
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
        std::ifstream tmpFile(filename); // this is equivalent to the above method
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
