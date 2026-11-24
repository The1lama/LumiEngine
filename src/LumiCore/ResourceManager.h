#pragma once


#include "logger/Logger.h"
#include <format>
#include <memory>
#include <string>
#include <unordered_map>

namespace LumiCore
{
  class Model;
  struct Vertex;
  struct Texture;
} // namespace LumiCore

namespace LumiCore::Resources
{
  struct AssetManagers;

  template <typename Resource>
  class ResourceManager
  {
  protected:
    std::unordered_map<std::string, std::unique_ptr<Resource>> m_resources;

    /**
     * Create new resource
     *
     * @ const std::string& resourcePath: The path to the resource we want to create
     */
    std::unique_ptr<Resource> CreateResource(const std::string& resourcePath);

  public:
    /**
     * Get a resource from the resource manager
     *
     * @ const std::string& resourcePath: the path to the resource we want to use
     *
     * @ return: a const reference of the resource
     */
    Resource& Get(std::string& resourcePath, Resources::AssetManagers& resources);

    /**
     * The resource will be moved to the resource manager. This will put the resource in a moved from state.
     * As we are moving the reference to the unsorted map
     *
     * @ std::string& resourcePath: The path name to the rescource, it it the key for accessing the resource later
     * @ Rescoure& ref: The type of resource we want to move to the unsorted map resource
     *
     * @ return bool: if it successfully moved the resource to the map
     */
    bool Create(std::string& resourcePath, Resource& ref);
    bool Create(std::string& resource, std::string& directory, Resource& ref);

    bool Contains(std::string& resourcePath) const;
  };

  template <typename Resource>
  inline bool ResourceManager<Resource>::Contains(std::string& resourcePath) const
  {
    return m_resources.contains(resourcePath);
  }

  template <typename Resource>
  inline bool ResourceManager<Resource>::Create(std::string& resourcePath, Resource& ref)
  {
    if (Contains(resourcePath)) return false;
    LOG_DEBUG("Make new Resouce: {}", resourcePath);
    m_resources.emplace(resourcePath, std::make_unique<Resource>(std::move(ref)));
    return true;
  }
  template <typename Resource>
  inline bool ResourceManager<Resource>::Create(std::string& resource, std::string& directory, Resource& ref)
  {
    if (Contains(directory + resource)) return false;
    LOG_DEBUG("Moing Resouce to resource manager: {}", resource);
    m_resources.emplace(directory + resource, std::make_unique<Resource>(std::move(ref)));
    return true;
  }

  template <typename Resource>
  inline Resource& ResourceManager<Resource>::Get(std::string& resourcePath, Resources::AssetManagers& resources)
  {
    if (!Contains(resourcePath))
    {
      // Generate new resource
      m_resources.emplace(resourcePath, std::make_unique<Resource>(resourcePath, resources));
    }

    return *m_resources.at(resourcePath);
  }


  struct AssetManagers
  {
    Resources::ResourceManager<LumiCore::Texture> Textures;
    Resources::ResourceManager<LumiCore::Model> Models;
  };

} // namespace LumiCore::Resources
