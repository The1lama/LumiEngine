#pragma once

#include <iostream>

#include "../Camera.h"
#include "../InputSystem.h"
#include <glm/vec3.hpp>
#include <string>

#include "../logger/Logger.h"


namespace PlayerStuff
{

  struct playerActions
  {
    float moveX = 0;
    float moveZ = 0;

    float yaw = 0;
    float pitch = 0;
    void Reset()
    {
      moveX = 0;
      moveZ = 0;
      yaw = 0;
      pitch = 0;
    }
  };

  struct Player
  {
  private:
    LumiCore::InputSystem::InputKeyBind m_keybindForward{LumiCore::InputSystem::EKeyCode::Keycode_W, 0};
    LumiCore::InputSystem::InputKeyBind m_keybindLeft{LumiCore::InputSystem::EKeyCode::Keycode_A, 0};
    LumiCore::InputSystem::InputKeyBind m_keybindBackward{LumiCore::InputSystem::EKeyCode::Keycode_S, 0};
    LumiCore::InputSystem::InputKeyBind m_keybindRight{LumiCore::InputSystem::EKeyCode::Keycode_D, 0};
    LumiCore::InputSystem::InputKeyBind m_keybindUp{LumiCore::InputSystem::EKeyCode::Keycode_E, 0};
    LumiCore::InputSystem::InputKeyBind m_keybindDown{LumiCore::InputSystem::EKeyCode::Keycode_Q, 0};

  public:
    LumiCore::Camera Camera;
    std::string ModelPath;
    glm::vec3 Position, Rotation;
    playerActions Action;
    unsigned int Index;

    Player(glm::vec3 startPos, glm::vec3 startRotation, std::string model, unsigned int index) :
        Position(startPos), Rotation(startRotation), ModelPath(model), Index(index)
    {
      Camera.SetLocation(startPos);
      Camera.SetYaw(startRotation.x);
      Camera.SetPitch(startRotation.y);

      InitializeInput(Index);
    }

    void GetPlayerInputAction(int playerIndex, std::string action)
    {
      if (action == "Forward")
      {
        Action.moveX = 1;
      }
      else if (action == "Backward")
      {
        Action.moveX = -1;
      }
      else if (action == "YawPlus")
      {
        Action.yaw = 1;
      }
      else if (action == "YawMinus")
      {
        Action.yaw = -1;
      }
      else if (action == "PitchPlus")
      {
        Action.pitch = 1;
      }
      else if (action == "PitchMinus")
      {
        Action.pitch = -1;
      }
    };

    void InitializeInput(const unsigned int playerIndex)
    {
      // only change input mapping if its the second player
      if (playerIndex != 0)
      {
        m_keybindForward = {LumiCore::InputSystem::EKeyCode::Keycode_Up, 0};
        m_keybindLeft = {LumiCore::InputSystem::EKeyCode::Keycode_Left, 0};
        m_keybindBackward = {LumiCore::InputSystem::EKeyCode::Keycode_Down, 0};
        m_keybindRight = {LumiCore::InputSystem::EKeyCode::Keycode_Right, 0};
        m_keybindUp = {LumiCore::InputSystem::EKeyCode::Keycode_PageUp, 0};
        m_keybindDown = {LumiCore::InputSystem::EKeyCode::Keycode_PageDown, 0};
      }

      // Inputs for player
      auto* inputManager = LumiCore::InputSystem::InputSystem::GetInstance();
      inputManager->BindKeybind(m_keybindForward, LumiCore::InputSystem::EKeyState::Hold,
                                [this]() { GetPlayerInputAction(0, "Forward"); });
      inputManager->BindKeybind(m_keybindBackward, LumiCore::InputSystem::EKeyState::Hold,
                                [this]() { GetPlayerInputAction(0, "Backward"); });
      inputManager->BindKeybind(m_keybindRight, LumiCore::InputSystem::EKeyState::Hold,
                                [this]() { GetPlayerInputAction(0, "YawPlus"); });
      inputManager->BindKeybind(m_keybindLeft, LumiCore::InputSystem::EKeyState::Hold,
                                [this]() { GetPlayerInputAction(0, "YawMinus"); });
      inputManager->BindKeybind(m_keybindUp, LumiCore::InputSystem::EKeyState::Hold,
                                [this]() { GetPlayerInputAction(0, "PitchPlus"); });
      inputManager->BindKeybind(m_keybindDown, LumiCore::InputSystem::EKeyState::Hold,
                                [this]() { GetPlayerInputAction(0, "PitchMinus"); });

      std::cout << this << std::endl;
    }

    void EndPlay()
    {
      auto* inputManager = LumiCore::InputSystem::InputSystem::GetInstance();
      inputManager->RemoveKeybind(m_keybindForward, LumiCore::InputSystem::EKeyState::Hold);
      inputManager->RemoveKeybind(m_keybindBackward, LumiCore::InputSystem::EKeyState::Hold);
      inputManager->RemoveKeybind(m_keybindRight, LumiCore::InputSystem::EKeyState::Hold);
      inputManager->RemoveKeybind(m_keybindLeft, LumiCore::InputSystem::EKeyState::Hold);
      inputManager->RemoveKeybind(m_keybindUp, LumiCore::InputSystem::EKeyState::Hold);
      inputManager->RemoveKeybind(m_keybindDown, LumiCore::InputSystem::EKeyState::Hold);
    }
  };

  // handle player input
} // namespace PlayerStuff


