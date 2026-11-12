#include "Logger.h"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

namespace LumiCoreLogger
{

std::unique_ptr<Logger> Logger::s_instance{nullptr};
std::mutex Logger::s_instanceMutexLock;

#pragma region Singleton
Logger::Logger() { ChangeLogFile(m_logFile); };
Logger::~Logger()
{
  if (m_currentFile.is_open())
  {
    m_currentFile.close();
  }
};

Logger* Logger::GetInstance()
{
  // the lock will be released when going out of scope
  std::lock_guard<std::mutex> lock(s_instanceMutexLock);

  // Create a new instance of the Logger class if the current instace is null
  if (!s_instance)
  {
    s_instance = std::make_unique<Logger>();
  }
  return s_instance.get();
};
#pragma endregion Singletion

#pragma region Log

void Logger::Log(const std::string& message, const ELogVerbose logLevel, const std::string& executionFile,
                 const int lineNumber)
{

  const std::string logInfo = LogFormat(message, logLevel, executionFile, lineNumber);

  LogToConsole(logInfo);

  if (m_currentFile.is_open())
  {
    LogToFile(logInfo);
  }
};

std::string Logger::LogFormat(const std::string& message, const ELogVerbose level, const std::string executionFile,
                              const int lineNumber)
{
  // Get current timestamp
  const auto localTime = std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()};
  const std::string formatedTime = std::format("{:%Y-%m-%d %T}", localTime);

  return std::string(
      std::format("[{}][{}][{}:{}] {}\n", formatedTime, EnumToString(level), executionFile, lineNumber, message));
}

void Logger::LogToConsole(const std::string& logInfo) { std::cout << logInfo; };
void Logger::LogToFile(const std::string& logInfo)
{
  if (!m_currentFile && !m_currentFile.is_open())
  {

    const std::string message = "Could not log to file, as it is not opened, OR there is no log file "
                                "to write to";
    std::string logInfo = LogFormat(message, ELogVerbose::CRITICAL, __FILE__, __LINE__);
    LogToConsole(logInfo);

    return;
  }
  // the lock will be released when going out of scope
  std::lock_guard<std::mutex> lock(s_instanceMutexLock);

  // Append to file with the log info
  m_currentFile << logInfo;
};

void Logger::ChangeLogFile(std::string filePath)
{
  auto workingDir = std::filesystem::current_path();

  auto thing = workingDir.append(filePath);
  std::string newFilePath = thing.string();

  std::filesystem::path pathObj(newFilePath);
  if (pathObj.has_parent_path())
  {
    std::filesystem::create_directories(pathObj.parent_path());
  }

  // check if the file is open
  if (m_currentFile.is_open())
  {
    m_currentFile.close();
  }
  // Check if there is already a file with name
  // if: Rename the old file
  if (FileExist(newFilePath))
  {
    RenameOldFile(newFilePath);
  }

  // Apppend to file automatiacly, will create file if not exsiting already.
  // then open the new file
  m_currentFile.open(newFilePath, std::ios::app);

  if (!m_currentFile.is_open())
  {
    const std::string message(std::format("Could not open log file: {}", newFilePath));
    auto logInfo = LogFormat(message, ELogVerbose::CRITICAL, __FILE__, __LINE__);
    LogToConsole(logInfo);
  }
  else
  {
    const std::string message(std::format("Switched log file to: {}", newFilePath));
    auto logInfo = LogFormat(message, ELogVerbose::INFO, __FILE__, __LINE__);
    LogToConsole(logInfo);
  }
}

bool Logger::FileExist(const std::string filePath)
{
  std::ifstream fiel(filePath);
  return fiel.good();
}

bool Logger::RenameOldFile(const std::string& oldFile)
{

  std::filesystem::path bakFile(oldFile);
  int fileAmount = GetFileAmountInDirectory(bakFile.parent_path().string());

  std::string bakFilePath = std::format("{}_bakNr_{}", bakFile.string().c_str(), std::to_string(fileAmount));
  std::rename(oldFile.c_str(), bakFilePath.c_str());

  const std::string message(std::format("Renamed old log file to: {}", bakFilePath));
  auto logInfo = LogFormat(message, ELogVerbose::INFO, __FILE__, __LINE__);
  return true;
}

int Logger::GetFileAmountInDirectory(const std::string& filePath)
{
  int fileAmount{0};
  for (const auto& entry : std::filesystem::directory_iterator(filePath))
  {
    ++fileAmount;
  }
  return fileAmount;
}

std::string Logger::EnumToString(const ELogVerbose level) const
{
  switch (level)
  {
  case ELogVerbose::DEBUG:
    return std::string("DEBUG");
  case ELogVerbose::INFO:
    return std::string("INFO");
  case ELogVerbose::WARNING:
    return std::string("WARNING");
  case ELogVerbose::CRITICAL:
    return std::string("CRITICAL");
  case ELogVerbose::ERROR:
    return std::string("ERROR");
  default:
    return std::string("NONE");
  }
}

#pragma endregion Log
} // namespace LumiCoreLogger
