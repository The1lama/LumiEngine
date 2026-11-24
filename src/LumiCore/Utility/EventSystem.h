#pragma once

#include <functional>
#include <utility>
#include <vector>

namespace LumiCore::Utility::Function
{
  /**
   * Create a event system
   *
   * @ typename... Args: The data type we want to use for when we are calling Notify(Args... args).
   * When instanciate the class it can be <>; <float>; <int, float, ...>
   * Example:
   * EventSystem<> event; // This would not use any parameter arguments in the Notify();
   * EventSystem<float> floatEvent; // This would use the float parameter in the Nofify(float);
   * EventSystem<int, std::string, ...> multiparamEvent; // This would send the amount of arguments in the Nofify(int,
   * std::string, ...);
   */
  template <typename... Args>
  class EventSystem
  {
  protected:
    /**
     * Returns the size of the amount of observers it has
     *
     * @ Return: The amount of observers the event system has
     */
    int GetSize()
    {
      return m_observers.size();
    }

  public:
    EventSystem<Args...>() = default;
    virtual ~EventSystem<Args...>() = default;

    /**
     * Attaches a observer to the event system
     *
     * @ Callable&& function: The function/Lambda that will be called when notify is called
     *
     * @ Return: index number in list it is in
     */
    template <typename Callable>
    unsigned int Attach(Callable&& function)
    {
      m_observers.push_back(std::forward<Callable>(function));
      return m_observers.size() - 1;
    }

    /**
     * Removes a observer on the event system
     *
     * @ unsigned int Index: The Index that we got from when we attached the observer function
     *
     * @ Return: id number in list
     */
    template <typename Callable>
    void Deatach(const unsigned int Index)
    {
      m_observers.erase(Index);
    }

    /**
     * Remove all the observers from the list
     */
    void Clear()
    {
      m_observers.clear();
    }

    /**
     * Will call all the observers that are subscribed to the event
     *
     * Args... args: Input any arguments if there was any, could be 0-n amount
     */
    virtual void Notify(Args... args)
    {
      // We reverse the observer list so that any newly added callbacks functions gets run next time
      const int ObserverLenght = GetSize();
      if (ObserverLenght <= 0) return;
      for (int i = ObserverLenght - 1; i >= 0; --i)
      {
        m_observers[i](args...);
      }
    }

  private:
    // std::function is a type that we can store, copy and invoke functions in.
    // Here we are storing all the observers for this event system, that we will
    // be calling later with Notify();
    std::vector<std::function<void(Args...)>> m_observers;
  };


} // namespace LumiCore::Utility::Function
