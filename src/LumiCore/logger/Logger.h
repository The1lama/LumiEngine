#pragma once

#include <fstream>
#include "../Utility/Singleton.h"

namespace LumiCore::Logger
{

// Log level enums
enum class ELogVerbose
{
  DEBUG,
  INFO,
  WARNING,
  CRITICAL,
  ERROR
};

class Logger : public Utility::Template::Singleton<Logger>
{
private:
  ELogVerbose m_minLogLevel = ELogVerbose::DEBUG;

  std::ofstream m_currentFile;

protected:
  const std::string m_logFile = "debug/log";

  bool FileExist(const std::string filePath);
  bool RenameOldFile(const std::string& oldFile);
  int GetFileAmountInDirectory(const std::string& filePath);

  void LogToConsole(const std::string& logInfo);
  void LogToFile(const std::string& logInfo);

  std::string EnumToString(const ELogVerbose level) const;
  std::string LogFormat(const std::string& message, const ELogVerbose level, const std::string executionFile,
                        const int lineNumber);

public:
  Logger();
  ~Logger();

  // Send out a log that prints out on console and writes to file
  void Log(const std::string& message, const ELogVerbose logLevel, const std::string& executionFile,
           const int lineNumber);

  // Used if we want to change the logfile
  void ChangeLogFile(std::string filePath);
};

} // namespace LumiCore::Logger

#define LOG(verbosity, message) LumiCore::Logger::Logger::GetInstance()->Log(message, verbosity, __FILE__, __LINE__)
#define LOG_DEBUG(message)                                                                                             \
  LumiCore::Logger::Logger::GetInstance()->Log(message, LumiCore::Logger::ELogVerbose::DEBUG, __FILE__, __LINE__)
#define LOG_INFO(message)                                                                                              \
  LumiCore::Logger::Logger::GetInstance()->Log(message, LumiCore::Logger::ELogVerbose::INFO, __FILE__, __LINE__)
#define LOG_WARNING(message)                                                                                           \
  LumiCore::Logger::Logger::GetInstance()->Log(message, LumiCore::Logger::ELogVerbose::WARNING, __FILE__, __LINE__)
#define LOG_CRITICAL(message)                                                                                          \
  LumiCore::Logger::Logger::GetInstance()->Log(message, LumiCore::Logger::ELogVerbose::CRITICAL, __FILE__, __LINE__)
#define LOG_ERROR(message)                                                                                             \
  LumiCore::Logger::Logger::GetInstance()->Log(message, LumiCore::Logger::ELogVerbose::ERROR, __FILE__, __LINE__)
