#pragma once


#include <memory>
#include <mutex>
namespace LumiCore::Utility::Function
{

  template <typename T>
  class Singleton
  {
  private:
    inline static std::unique_ptr<T> sInstance;
    inline static std::mutex sInstanceMutexLock;

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

    /**
     * Gets an instance of the class, if there is no instance we create a new one
     *
     * @ return T*: The T class we want as a singleton
     */
    static T* GetInstance()
    {
      std::lock_guard<std::mutex> lock(sInstanceMutexLock);
      if (!sInstance)
      {
        sInstance.reset(new T());
      }
      return sInstance.get();
    }
  };

} // namespace LumiCore::Utility::Function
