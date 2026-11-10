#pragma once

#include <fstream>
#include <memory>
#include <mutex>
namespace LumiCoreLogger {

// Log level enums
enum class LOGLEVEL { DEBUG, INFO, WARNING, CRITICAL, ERROR };

/*
 * This is logger class is a singleton design pattern, this is so we can access
 * this class from virtualy anywhere in the code.
 *
 * Works by just calling GetInstance();
 *
 */

class Logger {
private:
  LOGLEVEL m_minLogLevel = LOGLEVEL::DEBUG;

  static std::unique_ptr<Logger> s_instance;
  static std::mutex s_instanceMutexLock;

  std::ofstream m_currentFile;

protected:
  const std::string m_logFile = "debug\\log";

  bool FileExist(const std::string filePath);
  bool RenameOldFile(const std::string &oldFile);
  int GetFileAmountInDirectory(const std::string &filePath);

  void LogToConsole(const std::string &logInfo);
  void LogToFile(const std::string &logInfo);

  std::string EnumToString(const LOGLEVEL level) const;

public:
  Logger();
  ~Logger();
  // Delete copt/move constructors and assignment operatos
  // This is so we only have once instance off the logger class
  // Logger(const Logger &other) = delete;
  // Logger &operator=(const Logger &other) = delete;
  // Logger(Logger &&other) = delete;
  // Logger &operator=(Logger &&) = delete;

  // We call this function to get access to the logger, we check first if there
  // is already a singleton, if there is non we create it
  static Logger *GetInstance();

  // Send out a log that prints out on console and writes to file
  void Log(const std::string &message, const LOGLEVEL logLevel,
           const std::string &executionFile, const int lineNumber);

  // used if we want to change the log file
  void ChangeLogFile(std::string filePath);
};

} // namespace LumiCoreLogger

#define LOG(verbosity, message)                                                \
  LumiCoreLogger::Logger::GetInstance()->Log(message, verbosity, __FILE__,     \
                                             __LINE__)
#define LOG_DEBUG(message)                                                     \
  LumiCoreLogger::Logger::GetInstance()->Log(                                  \
      message, LumiCoreLogger::LOGLEVEL::DEBUG, __FILE__, __LINE__)
#define LOG_INFO(message)                                                      \
  LumiCoreLogger::Logger::GetInstance()->Log(                                  \
      message, LumiCoreLogger::LOGLEVEL::INFO, __FILE__, __LINE__)
#define LOG_WARNING(message)                                                   \
  LumiCoreLogger::Logger::GetInstance()->Log(                                  \
      message, LumiCoreLogger::LOGLEVEL::WARNING, __FILE__, __LINE__)
#define LOG_CRITICAL(message)                                                  \
  LumiCoreLogger::Logger::GetInstance()->Log(                                  \
      message, LumiCoreLogger::LOGLEVEL::CRITICAL, __FILE__, __LINE__)
#define LOG_ERROR(message)                                                     \
  LumiCoreLogger::Logger::GetInstance()->Log(                                  \
      message, LumiCoreLogger::LOGLEVEL::ERROR, __FILE__, __LINE__)
