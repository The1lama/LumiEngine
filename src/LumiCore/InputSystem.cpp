#include "InputSystem.h"
#include <format>
#include "GLFW/glfw3.h"
#include "logger/Logger.h"

namespace LumiCore::InputSystem
{
InputSystem::InputSystem(GLFWwindow* window)
{
  Initialize(window);
}
InputSystem::~InputSystem()
{
  Cleanup();
}
void InputSystem::Initialize(GLFWwindow* window)
{
  m_window = window;
  glfwSetWindowUserPointer(m_window, this);
  glfwSetKeyCallback(window, KeyCallbackStatic);
}

void InputSystem::Cleanup()
{
  if (m_window) m_window = nullptr;
}

void InputSystem::KeyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (action != GLFW_PRESS && action != GLFW_RELEASE) return;
  InputSystem* instance = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
  if (instance) instance->ProcessInput(key, scancode, action, mods);
}

void InputSystem::ProcessInput(int key, int scancode, int action, int mods)
{
  // bulid the keyinput
  InputKeyBind inputCombo;
  inputCombo.baseKey = (EKeyCode)key;
  inputCombo.modifierFlags = mods;

  // send the notification event if there is a correct input action
  LOG_DEBUG(std::format("Action press: {}", action));
  if (action == GLFW_PRESS && m_keybindsPressed.contains(inputCombo))
  {
    m_keybindsPressed[inputCombo].Notify();
  }
  else if (action == GLFW_RELEASE && m_keybindsReleased.contains(inputCombo))
  {
    m_keybindsReleased[inputCombo].Notify();
  }
}

void InputSystem::AddKeybind(const InputKeyBind& keyBind)
{
  m_keybindsPressed.try_emplace(keyBind, Utility::Function::EventSystem<>());
  m_keybindsReleased.try_emplace(keyBind, Utility::Function::EventSystem<>());
}

void InputSystem::RemoveKeybind(const InputKeyBind& keyBind)
{
  m_keybindsPressed.erase(keyBind);
  m_keybindsReleased.erase(keyBind);
}

bool InputSystem::KeybindExist(const InputKeyBind& keyBind) const
{
  return m_keybindsPressed.contains(keyBind);
}

} // namespace LumiCore::InputSystem
