#pragma once


// forward declarations
#include "Utility/EventSystem.h"

#include "Utility/Singleton.h"

#include <cstdint>
#include <unordered_map>

struct GLFWwindow;


namespace LumiCore::InputSystem
{

struct InputAction
{

  // What is should hold
  //
  // KeyPress
  // WasPressedThisFrame
  // IsPressed
  // WasReleasedThisFrame
};

enum class EKeyCode : uint16_t
{
  Keycode_Space = 32,
  Keycode_Apostrophe = 39, /* ' */
  Keycode_Comma = 44, /* , */
  Keycode_Minus = 45, /* - */
  Keycode_Period = 46, /* . */
  Keycode_Slash = 47, /* / */
  Keycode_Key0 = 48,
  Keycode_Key1 = 49,
  Keycode_Key2 = 50,
  Keycode_Key3 = 51,
  Keycode_Key4 = 52,
  Keycode_Key5 = 53,
  Keycode_Key6 = 54,
  Keycode_Key7 = 55,
  Keycode_Key8 = 56,
  Keycode_Key9 = 57,
  Keycode_Semicolon = 59, /* ; */
  Keycode_Equal = 61, /* = */
  Keycode_A = 65,
  Keycode_B = 66,
  Keycode_C = 67,
  Keycode_D = 68,
  Keycode_E = 69,
  Keycode_F = 70,
  Keycode_G = 71,
  Keycode_H = 72,
  Keycode_I = 73,
  Keycode_J = 74,
  Keycode_K = 75,
  Keycode_L = 76,
  Keycode_M = 77,
  Keycode_N = 78,
  Keycode_O = 79,
  Keycode_P = 80,
  Keycode_Q = 81,
  Keycode_R = 82,
  Keycode_S = 83,
  Keycode_T = 84,
  Keycode_U = 85,
  Keycode_V = 86,
  Keycode_W = 87,
  Keycode_X = 88,
  Keycode_Y = 89,
  Keycode_Z = 90,
  Keycode_LeftBracket = 91, /* [ */
  Keycode_Backslash = 92, /* \ */
  Keycode_RightBracket = 93, /* ] */
  Keycode_GraveAccent = 96, /* ` */
  Keycode_World1 = 161, /* non-US #1 */
  Keycode_World2 = 162, /* non-US #2 */
  Keycode_Escape = 256,
  Keycode_Enter = 257,
  Keycode_Tab = 258,
  Keycode_Backspace = 259,
  Keycode_Insert = 260,
  Keycode_Delete = 261,
  Keycode_Right = 262,
  Keycode_Left = 263,
  Keycode_Down = 264,
  Keycode_Up = 265,
  Keycode_PageUp = 266,
  Keycode_PageDown = 267,
  Keycode_Home = 268,
  Keycode_End = 269,
  Keycode_CapsLock = 280,
  Keycode_ScrollLock = 281,
  Keycode_NumLock = 282,
  Keycode_PrintScreen = 283,
  Keycode_Pause = 284,
  Keycode_F1 = 290,
  Keycode_F2 = 291,
  Keycode_F3 = 292,
  Keycode_F4 = 293,
  Keycode_F5 = 294,
  Keycode_F6 = 295,
  Keycode_F7 = 296,
  Keycode_F8 = 297,
  Keycode_F9 = 298,
  Keycode_F10 = 299,
  Keycode_F11 = 300,
  Keycode_F12 = 301,
  Keycode_F13 = 302,
  Keycode_F14 = 303,
  Keycode_F15 = 304,
  Keycode_F16 = 305,
  Keycode_F17 = 306,
  Keycode_F18 = 307,
  Keycode_F19 = 308,
  Keycode_F20 = 309,
  Keycode_F21 = 310,
  Keycode_F22 = 311,
  Keycode_F23 = 312,
  Keycode_F24 = 313,
  Keycode_F25 = 314,
  Keycode_Kp0 = 320,
  Keycode_Kp1 = 321,
  Keycode_Kp2 = 322,
  Keycode_Kp3 = 323,
  Keycode_Kp4 = 324,
  Keycode_Kp5 = 325,
  Keycode_Kp6 = 326,
  Keycode_Kp7 = 327,
  Keycode_Kp8 = 328,
  Keycode_Kp9 = 329,
  Keycode_KpDecimal = 330,
  Keycode_KpDivide = 331,
  Keycode_KpMultiply = 332,
  Keycode_KpSubtract = 333,
  Keycode_KpAdd = 334,
  Keycode_KpEnter = 335,
  Keycode_KpEqual = 336,
  Keycode_LeftShift = 340,
  Keycode_LeftControl = 341,
  Keycode_LeftAlt = 342,
  Keycode_LeftSuper = 343,
  Keycode_RightShift = 344,
  Keycode_RightControl = 345,
  Keycode_RightAlt = 346,
  Keycode_RightSuper = 347,
  Keycode_Menu = 348,
};

enum class EModifierFlags : uint8_t
{
  None = 0,
  Shift = 1 << 0, // 1
  Ctrl = 1 << 1, // 2
  Alt = 1 << 2, // 4
  Super = 1 << 3, // 8
};

inline constexpr EModifierFlags operator|(EModifierFlags a, EModifierFlags b)
{
  return static_cast<EModifierFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(a));
}

struct InputKeyBind
{
  EKeyCode baseKey;
  uint8_t modifierFlags;

  bool operator==(const InputKeyBind& other) const = default;
};

} // namespace LumiCore::InputSystem


// the hash function override needs to be in it's own namespace for std right after the operator==
// becuase we need to override the function for hash operation. Thaths why we have also split the
// InputSystem namespace in two
namespace std
{
/**
 * This function extends the has function so we can use our custom made InputKeyBind struct with an unorderd_map,
 * as the struct would not then be able to hash itself and be unable to be used in a hash map
 */
template <>
struct hash<LumiCore::InputSystem::InputKeyBind>
{
  size_t operator()(const LumiCore::InputSystem::InputKeyBind Bind) const
  {
    // create a hash key from the baseKeyInput and the same with mod
    size_t keyHash = std::hash<int>{}(static_cast<int>(Bind.baseKey));
    size_t modHash = std::hash<uint8_t>{}(static_cast<int>(Bind.modifierFlags));

    // return the combined hash value for the struct
    return keyHash ^ (modHash << 1);
  }
};
} // namespace std


namespace LumiCore::InputSystem
{
class InputSystem : public Utility::Function::Singleton<InputSystem>
{
private:
  const std::string Directory = "KeyInputs/";

  GLFWwindow* m_window;
  std::string m_keyMapName;

  static void KeyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods);
  void ProcessInput(int key, int scancode, int action, int mods);

  // stores all the keybinds that we have and a event system for them to process
  std::unordered_map<InputKeyBind, Utility::Function::EventSystem<>> m_keybinds;

  // stores hold keypresses
  // std::unordered_map<int, int> m_pressedKeys;
  // InputKeyBind m_pressedKeys[348]; // The amount of keycodes that glfw has defined
  // https://www.glfw.org/docs/latest/group__keys.html

  void Initialize(GLFWwindow* window);
  void Cleanup();

protected:
  void GenerateKeyMap(GLFWwindow* window);


public:
  InputSystem(GLFWwindow* window);
  ~InputSystem();


  void AddKeybind(const EKeyCode KeyCode, const uint8_t ModifierFlags);

  template <typename Callable>
  bool BindKeybind(const EKeyCode KeyCode, const uint8_t ModifierFlags, Callable&& function);

  void RemoveKeybind(const EKeyCode KeyCode, const uint8_t ModifierFlags);
};

template <typename Callable>
bool InputSystem::BindKeybind(const EKeyCode KeyCode, const uint8_t ModifierFlags, Callable&& function)
{
  InputKeyBind buildKeybind{KeyCode, ModifierFlags};

  if (!m_keybinds.contains(buildKeybind)) return false;

  m_keybinds[buildKeybind].Attach(function);
  return true;
}


} // namespace LumiCore::InputSystem


/*

bool processInput(GLFWwindow* window, bool& bToggleColor, bool& bSwitchDrawMode, bool& bSwitchProjectionMatrix)
{
  bool hasPressed = false;

  // Change color on object //
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    hasPressed = true;
    bToggleColor = false;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
  {
    bToggleColor = true;
  }

  // Draw mode switch //
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
  {
    hasPressed = true;
    if (bSwitchDrawMode == false) SwitchRenderingMode();
    bSwitchDrawMode = true;
  }
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
  {
    bSwitchDrawMode = false;
  }

  // Projection matrix switch //
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
  {
    hasPressed = true;
    bSwitchProjectionMatrix = true;
  }
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
  {
    bSwitchProjectionMatrix = false;
  }

  return hasPressed;
}

*/
