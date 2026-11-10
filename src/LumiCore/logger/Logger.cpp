#include "Logger.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

namespace LumiCoreLogger {

std::unique_ptr<Logger> Logger::s_instance{nullptr};
std::mutex Logger::s_instanceMutexLock;

#pragma region Singleton
Logger::Logger() { ChangeLogFile(m_logFile); };
Logger::~Logger() {
  if (m_currentFile.is_open()) {
    m_currentFile.close();
  }
};

Logger *Logger::GetInstance() {
  // the lock will be released when going out of scope
  std::lock_guard<std::mutex> lock(s_instanceMutexLock);

  // create a new instance of the Logger class if the current instace is null
  if (!s_instance) {
    s_instance = std::make_unique<Logger>();
  }
  return s_instance.get();
};
#pragma endregion Singletion

#pragma region Log

void Logger::Log(const std::string &message, const LOGLEVEL logLevel,
                 const std::string &executionFile, const int lineNumber) {

  // Get current time stamp
  const auto localTime = std::chrono::zoned_time{
      std::chrono::current_zone(), std::chrono::system_clock::now()};
  const std::string formatedTime = std::format("{:%Y-%m-%d %T}", localTime);

  const std::string logInfo(std::format("[{}][{}][{}:{}] {}\n", formatedTime,
                                        EnumToString(logLevel), executionFile,
                                        lineNumber, message));

  LogToConsole(logInfo);

  if (m_currentFile.is_open()) {
    LogToFile(logInfo);
  }
};

void Logger::LogToConsole(const std::string &logInfo) { std::cout << logInfo; };
void Logger::LogToFile(const std::string &logInfo) {
  // the lock will be released when going out of scope
  std::lock_guard<std::mutex> lock(s_instanceMutexLock);

  // Append to file with the log info
  m_currentFile << logInfo;
};

std::string Logger::EnumToString(const LOGLEVEL level) const {
  switch (level) {
  case LOGLEVEL::DEBUG:
    return std::string("DEBUG");
  case LOGLEVEL::INFO:
    return std::string("INFO");
  case LOGLEVEL::WARNING:
    return std::string("WARNING");
  case LOGLEVEL::CRITICAL:
    return std::string("CRITICAL");
  case LOGLEVEL::ERROR:
    return std::string("ERROR");
  default:
    return std::string("NONE");
  }
}

void Logger::ChangeLogFile(std::string filePath) {
  auto workingDir = std::filesystem::current_path();

  auto thing = workingDir.append(filePath);
  std::string newFilePath = thing.string();

  std::filesystem::path pathObj(newFilePath);
  if (pathObj.has_parent_path()) {
    std::filesystem::create_directories(pathObj.parent_path());
  }

  // check if the file is open
  if (m_currentFile.is_open()) {
    m_currentFile.close();
  }
  // Check if there is already a file with name
  // if: Rename the old file
  if (FileExist(newFilePath)) {
    std::cout << std::format("FileExisted \n");
    RenameOldFile(newFilePath);
  }

  // Apppend to file automatiacly, will create file if not exsiting already.
  // then open the new file
  m_currentFile.open(newFilePath, std::ios::app);

  if (!m_currentFile.is_open()) {
    std::cout << std::format("ERROR: Courld not open log file \n");
  } else {
    std::cout << std::format("Current Debug file location is at {} \n",
                             newFilePath);
  }
}

bool Logger::FileExist(const std::string filePath) {
  std::ifstream fiel(filePath);
  return fiel.good();
}

bool Logger::RenameOldFile(const std::string &oldFile) {

  std::filesystem::path bakFile(oldFile);
  int fileAmount = GetFileAmountInDirectory(bakFile.parent_path().string());

  std::string bakFilePath = std::format("{}_bakNr_{}", bakFile.string().c_str(),
                                        std::to_string(fileAmount));
  std::rename(oldFile.c_str(), bakFilePath.c_str());

  std::cout << std::format("Renamed file to {} \n", bakFilePath);
  return true;
}

int Logger::GetFileAmountInDirectory(const std::string &filePath) {
  int fileAmount{0};
  for (const auto &entry : std::filesystem::directory_iterator(filePath)) {
    ++fileAmount;
  }
  return fileAmount;
}
#pragma endregion Log
} // namespace LumiCoreLogger
