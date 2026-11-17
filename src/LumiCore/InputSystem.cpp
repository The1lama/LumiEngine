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
  if (action = !GLFW_PRESS && action != GLFW_REPEAT) return;
  InputSystem* instance = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
  if (instance) instance->ProcessInput(key, scancode, action, mods);
}

void InputSystem::ProcessInput(int key, int scancode, int action, int mods)
{
  //  LOG_INFO(
  //      std::format("KeyPressed in callback. Key: {} scancode: {}, action: {}, mods: {}", key, scancode, action,
  //      mods));

  // bulid the keyinput
  InputKeyBind inputCombo;
  inputCombo.baseKey = (EKeyCode)key;
  inputCombo.modifierFlags = mods;

  // send the notification event if there is a correct input action
  if (m_keybinds.contains(inputCombo))
  {
    m_keybinds[inputCombo].Notify();
  }
}

void InputSystem::AddKeybind(const EKeyCode KeyCode, const uint8_t ModifierFlags)
{
  InputKeyBind buildKeybind{KeyCode, ModifierFlags};

  m_keybinds.try_emplace(buildKeybind, Utility::Function::EventSystem<>());
}

void InputSystem::RemoveKeybind(const EKeyCode KeyCode, const uint8_t ModifierFlags)
{
  InputKeyBind buildKeybind{KeyCode, ModifierFlags};

  m_keybinds.try_emplace(buildKeybind, Utility::Function::EventSystem<>());
}

} // namespace LumiCore::InputSystem
