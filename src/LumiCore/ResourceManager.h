#pragma once


#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace LumiCore
{
class Model;
class Vertex;
class Texture;
} // namespace LumiCore

namespace LumiCore::Resources
{

class ResourceManager
{
protected:
  std::unordered_map<std::string, std::weak_ptr<Model>> m_modelResources;

  std::shared_ptr<Model> GenerateModel(const std::string& path);
  std::shared_ptr<Model> GenerateObject(const std::string& path, const std::vector<LumiCore::Texture>& textures);
  std::shared_ptr<Model> CustomObjectLoader(const std::string& path, std::vector<LumiCore::Vertex>& vertices,
                                            std::vector<unsigned int>& indices,
                                            const std::vector<LumiCore::Texture>& textures);

public:
  /* Will return a model of the given path.
   *
   * If it has a valid file extension and path
   *
   * If the model has already been created we just return the model
   * else we create a new one
   */
  std::shared_ptr<Model> GetModel(const std::string& path, const std::vector<LumiCore::Texture>& textures);
};

} // namespace LumiCore::Resources
