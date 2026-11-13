#pragma once

#include <math.h>
#include "../Mesh.h"
#include "../logger/Logger.h"
#include "glm/ext/scalar_constants.hpp"
#include "glm/trigonometric.hpp"

namespace LumiCore3D
{

class Sphere : public LumiCore::Mesh
{
  public:
  Sphere(const int rings, const int slices, const std::vector<LumiCore::Texture> textures) :
      LumiCore::Mesh(GenerateMeshData(rings, slices, textures), GL_TRIANGLES) {};

  Sphere(const std::vector<LumiCore::Texture> textures) :
      LumiCore::Mesh(GenerateMeshData(50, 55, textures), GL_TRIANGLES) {};

  private:
  static inline LumiCore::MeshData GenerateMeshData(const int rings, const int slices,
                                                    const std::vector<LumiCore::Texture> textures)
  {
    LumiCore::MeshData data;

    if (rings < 3)
    {
      LOG_CRITICAL("No sphere generated. Rings value < 3");
      return data;
    }
    if (slices < 4)
    {
      LOG_CRITICAL("No sphere generated. Rings value < 4");
      return data;
    }

    std::vector<LumiCore::Vertex> vertices;
    std::vector<unsigned int> indices;

    Sphere::GenerateVertexArray(vertices, rings, slices);
    Sphere::GenerateIndicies(vertices, indices, rings, slices);

    data.vertecies = std::move(vertices);
    data.indices = std::move(indices);
    data.textures = std::move(textures);

    return data;
  };

  static inline bool GenerateVertexArray(std::vector<LumiCore::Vertex>& vertices, const int rings, const int slices)
  {
    constexpr float PIE = 3.1415f;

    // Creates how many sides the sphere should have in a circle
    float angle_step = 360.0f / slices;
    float current_angle = 0.0f; // caches the current used angle

    // Vertices
    float y_step = 2.0f / (rings - 1);
    float y_pos = 1.0f;

    //
    for (int ring = 0; ring < rings; ring++)
    {

      int iteration_slices = slices;
      if (ring == 0 || ring == rings - 1)
      {
        iteration_slices = 1;
      }

      for (int i = 0; i < iteration_slices; i++)
      {

        float radius = sinf(acos(y_pos));

        float x_pos = cosf(current_angle * PIE / 180) * radius;
        float z_pos = sinf(current_angle * PIE / 180) * radius;

        const auto vertexPos = glm::vec3(x_pos, y_pos, z_pos);
        const auto centerPos = glm::vec3(0, 0, 0);

        const glm::vec3 vertexNormal = CalculateSurfaceNormal(vertexPos, centerPos, radius);

        const glm::vec2 vertexUvCoord = CalculateUvPosition(vertexPos, centerPos, radius);

        vertices.push_back(LumiCore::Vertex(vertexPos, vertexNormal, vertexUvCoord));

        current_angle += angle_step;
      }

      y_pos -= y_step;
      current_angle = 0.0f;
    }
    return true;
  }

  static inline bool GenerateIndicies(const std::vector<LumiCore::Vertex> vertices, std::vector<unsigned int>& indices,
                                      const int rings, const int slices)
  {
    // Indices
    unsigned int top_most_index = 0;
    unsigned int bottom_most_index = vertices.size() - 1;

    for (int i = 0; i < rings - 1; i++)
    {

      for (int j = 0; j < slices; j++)
      {

        unsigned int tl_index = (i - 1) * slices + j + 1;
        unsigned int tr_index = (i - 1) * slices + j + 1 + 1;
        unsigned int bl_index = (i + 0) * slices + j + 1;
        unsigned int br_index = (i + 0) * slices + j + 1 + 1;

        if (j == slices - 1)
        { // For the seam
          tr_index = (i - 2) * slices + j + 1 + 1;
          br_index = (i - 1) * slices + j + 1 + 1;
        }

        // Winding order is now counter clock wise
        if (i == 0)
        { // First ring

          indices.push_back(top_most_index);
          indices.push_back(bl_index);
          indices.push_back(br_index);
        }
        else if (i == rings - 2)
        { // Last ring

          indices.push_back(tl_index);
          indices.push_back(bottom_most_index);
          indices.push_back(tr_index);
        }
        else
        {

          indices.push_back(tl_index);
          indices.push_back(bl_index);
          indices.push_back(br_index);

          indices.push_back(tl_index);
          indices.push_back(br_index);
          indices.push_back(tr_index);
        }
      }
    }
    return true;
  };

  // Calculate a surface normal vector of a sphere
  static inline glm::vec3 CalculateSurfaceNormal(const glm::vec3 vertexPos, const glm::vec3 sphereCenter,
                                                 const float radius)
  {
    return glm::normalize(vertexPos - sphereCenter);
  }

  static inline glm::vec2 CalculateUvPosition(const glm::vec3 vertexPos, const glm::vec3 sphereCenter,
                                              const float radius)
  {
    // normalize the vertex position
    const auto normPos = CalculateSurfaceNormal(vertexPos, sphereCenter, radius);

    // Calculate the projection mapping using a standard spherical function
    const float u = 0.5f + (glm::atan(normPos.z, normPos.x)) / (2 * glm::pi<float>());
    const float v = 0.5f + (glm::asin(normPos.y) / glm::pi<float>());
    return glm::vec2(u, v);
  };

  // It's not the nicest sphere, nor the nicest code, but it sort of works.
};

} // namespace LumiCore3D
