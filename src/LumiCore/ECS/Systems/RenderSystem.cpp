#include "RenderSystem.h"

#include "../../ComponentTypes.h"
#include "../../Model.h"
#include "../../ResourceManager.h"
#include "../../Shader.h"
#include "../EcsFacade.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

namespace LumiCore::ECS::Systems
{


  constexpr glm::vec3 forward(1.0f, 0.0f, 0.0f);
  constexpr glm::vec3 up(0.0f, 1.0f, 0.0f);
  constexpr glm::vec3 right(0.0f, 0.0f, 1.0f);


  void RenderSystem::Render(Shader& shader, Resources::AssetManagers& resources)
  {
    auto* facade = ECSFacade::GetInstance();

    for (auto const& entity : Entities)
    {
      auto& modelComponent = facade->GetComponent<ModelComponent>(entity);
      auto& transformComponent = facade->GetComponent<Transform>(entity);

      Texture texture;
      if (facade->EntityHasComponent<TextureComponent>(entity))
      {
        auto& textureComponent = facade->GetComponent<TextureComponent>(entity);
        // do override the model texture somehow
        texture = resources.Textures.Get(textureComponent.Path, resources);
      }

      glActiveTexture(GL_TEXTURE0);

      glm::mat4 modelMatrix = glm::mat4(1.0f);

      modelMatrix = glm::translate(modelMatrix, transformComponent.Position);
      modelMatrix = glm::rotate(modelMatrix, glm::radians(transformComponent.Rotation.x), forward);
      modelMatrix = glm::rotate(modelMatrix, glm::radians(transformComponent.Rotation.y), up);
      modelMatrix = glm::rotate(modelMatrix, glm::radians(transformComponent.Rotation.z), right);

      modelMatrix = glm::scale(modelMatrix, transformComponent.Scale);

      glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(glm::mat3(modelMatrix))));
      shader.SetMatrix3("u_NormalMatrix", normalMatrix);
      shader.SetMatrix4("u_Model", modelMatrix);

      // Get model from resource manager
      auto& model = resources.Models.Get(modelComponent.Path, resources);

      model.Draw(shader);
    }
  }


} // namespace LumiCore::ECS::Systems
