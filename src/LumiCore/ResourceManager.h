#pragma once


#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace LumiCore
{
  class Model;
  struct Vertex;
  struct Texture;
} // namespace LumiCore

namespace LumiCore::Resources
{

  class ResourceManager
  {
  protected:
    std::unordered_map<std::string, std::unique_ptr<Model>> m_modelResources;
    std::vector<Texture> m_textureResources;

    std::unique_ptr<Model> GenerateModel(const std::string& path);
    std::unique_ptr<Model> GenerateObject(const std::string& path, const std::vector<LumiCore::Texture>& textures);
    std::unique_ptr<Model> CustomObjectLoader(const std::string& path, std::vector<LumiCore::Vertex>& vertices,
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
    Model* GetModel(const std::string& path, const std::vector<LumiCore::Texture>& textures);

    /**
     * Generate texture and return the index
     *
     * @ const std::string& path: the path the texture should generate from
     *
     * @ Return: The index the texture is at
     *
     */
    unsigned int GenerateTexture(const std::string& path, const std::string& typeOfMaterial = "material");
    /**
     * Get a texture from the gerenrated textures.
     *
     * @ const int index: The index of the texture that we got from GenerateTexture()
     *
     * @ Return: The texture
     *
     */
    const Texture* GetTextureById(const unsigned int index);
  };

} // namespace LumiCore::Resources
