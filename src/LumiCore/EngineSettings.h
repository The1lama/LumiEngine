#pragma once

#include <string>
#include <vector>

namespace LumiEngine
{
  // If we should use our own created .obj loader or if we should use Assimp for object loading
  static constexpr bool useCustomObjectLoader = false;

  // A validation vector of all the
  static const std::vector<std::string> ValidObjectFileExtentsions = {".obj"};

  static int windowWidth = 640;
  static int windowHeight = 480;
  static const char* windowTitle = "LumiEngine";


} // namespace LumiEngine
