#pragma once


#include <memory>
#include <mutex>
namespace LumiCore::Utility::Template
{

template <typename T>
class Singleton
{
private:
  inline static std::unique_ptr<T> s_instance;
  inline static std::mutex s_instanceMutexLock;

protected:
  Singleton() = default;
  virtual ~Singleton() = default;

public:
  // Delete copy/move constructors and assignment operator
  // This is so we only have once instance off the logger class
  Singleton(const Singleton& other) = delete;
  Singleton& operator=(const Singleton& other) = delete;
  Singleton(Singleton&& other) = delete;
  Singleton& operator=(Singleton&&) = delete;

  // We call this function to get access to the logger, we check first if there
  // is already a singleton, if there is non we create it
  inline static T* GetInstance()
  {
    std::lock_guard<std::mutex> lock(s_instanceMutexLock);
    if (!s_instance)
    {
      s_instance.reset(new T());
    }
    return s_instance.get();
  }
};

} // namespace LumiCore::Utility::Template
