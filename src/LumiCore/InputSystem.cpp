#include "InputSystem.h"
#include "GLFW/glfw3.h"
#include "Utility.h"
#include "logger/Logger.h"
#include <format>

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
    // Do not call this if we are holding the key, as we are processing it in our update
    if (action == GLFW_REPEAT) return;
    InputSystem* instance = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (instance) instance->ProcessInput(key, scancode, action, mods);
  }

  void InputSystem::ProcessInput(int key, int scancode, int action, int mods)
  {
    // build the key input
    InputKeyBind inputCombo;
    inputCombo.baseKey = (EKeyCode)key;
    inputCombo.modifierFlags = mods;

    // Send the notification event if there is a correct input action
    LOG_DEBUG("Action press: {}", action);

    if (action == GLFW_PRESS) m_currentKeysDown.emplace_back(inputCombo);
    if (action == GLFW_RELEASE) std::erase(m_currentKeysDown, inputCombo);
  }

  void InputSystem::RemoveKeybind(const InputKeyBind& keyBind, const EKeyState state)
  {
    switch (state)
    {
    case EKeyState::Released:
      m_keybindsReleased.erase(keyBind);
    case EKeyState::Pressed:
      m_keybindsPressed.erase(keyBind);
    case EKeyState::Hold:
      m_keybindsHold.erase(keyBind);
    }
  }

  bool InputSystem::KeybindExist(const InputKeyBind& keyBind, const EKeyState state) const
  {
    switch (state)
    {
    case EKeyState::Released:
      return m_keybindsReleased.contains(keyBind);
    case EKeyState::Pressed:
      return m_keybindsPressed.contains(keyBind);
    case EKeyState::Hold:
      return m_keybindsHold.contains(keyBind);
    }
  }

  void InputSystem::Update()
  {
    std::vector<InputKeyBind> proccessedKeys;

    // compare current and previus keys
    for (unsigned int i = 0; i < m_currentKeysDown.size(); ++i)
    {
      InputKeyBind& keydown = m_currentKeysDown[i];

      // if the current key down was inputed last frame call hold key event
      if (Utility::ContainsInVector(m_previousKeysDown, keydown)) m_keybindsHold[keydown].Notify();
      //
      // else it was pressed this frame, and call press key event
      else m_keybindsPressed[keydown].Notify();
      //
      // add the key to proccessed keys, that we use later for checking if keys has been released
      proccessedKeys.emplace_back(keydown);
    }

    for (unsigned int i = 0; i < m_previousKeysDown.size(); ++i)
    {
      InputKeyBind& keydown = m_previousKeysDown[i];

      // check if the currently checked key is in proccessed key vector.
      // If is NOT, then it means that we have released the key and should call Release
      if (!Utility::ContainsInVector(proccessedKeys, keydown))
      {
        m_keybindsReleased[keydown].Notify();
      }
    }

    // override the previous keys down chache for the next frame to compare the inputs
    m_previousKeysDown = m_currentKeysDown;
  }

} // namespace LumiCore::InputSystem
