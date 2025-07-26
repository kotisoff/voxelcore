#include "HandsRenderer.hpp"

#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ModelBatch.hpp"
#include "assets/Assets.hpp"
#include "content/Content.hpp"
#include "graphics/core/Shader.hpp"
#include "graphics/commons/Model.hpp"
#include "items/Inventory.hpp"
#include "items/ItemStack.hpp"
#include "items/ItemDef.hpp"
#include "objects/Player.hpp"
#include "objects/rigging.hpp"
#include "world/Level.hpp"
#include "window/Camera.hpp"
#include "window/Window.hpp"

using namespace rigging;

HandsRenderer::HandsRenderer(
    const Assets& assets,
    const Level& level,
    const Player& player,
    ModelBatch& modelBatch,
    std::shared_ptr<Skeleton> skeleton
)
    : assets(assets),
      level(level),
      player(player),
      modelBatch(modelBatch),
      skeleton(std::move(skeleton)) {
}

void HandsRenderer::renderHands(
    const Camera& camera, float delta
) {
    // configure model matrix
    const glm::vec3 itemOffset(0.06f, 0.035f, -0.1);

    static glm::mat4 prevRotation(1.0f);

    const float speed = 24.0f;
    glm::mat4 matrix = glm::translate(glm::mat4(1.0f), itemOffset);
    matrix = glm::scale(matrix, glm::vec3(0.1f));
    glm::mat4 rotation = camera.rotation;

    // rotation interpolation
    glm::quat rot0 = glm::quat_cast(prevRotation);
    glm::quat rot1 = glm::quat_cast(rotation);
    glm::quat finalRot =
        glm::slerp(rot0, rot1, static_cast<float>(delta * speed));
    rotation = glm::mat4_cast(finalRot);
    prevRotation = rotation;
    
    // building matrix
    matrix = rotation * matrix *
             glm::rotate(
                 glm::mat4(1.0f), -glm::pi<float>() * 0.5f, glm::vec3(0, 1, 0)
             );

    // getting offset
    glm::vec3 cameraRotation = player.getRotation();
    auto offset = -(camera.position - player.getPosition());
    float angle = glm::radians(cameraRotation.x - 90);
    float cos = glm::cos(angle);
    float sin = glm::sin(angle);

    float newX = offset.x * cos - offset.z * sin;
    float newZ = offset.x * sin + offset.z * cos;
    offset = glm::vec3(newX, offset.y, newZ);
    matrix = matrix * glm::translate(glm::mat4(1.0f), offset);

    // get current chosen item
    auto indices = level.content.getIndices();
    const auto& inventory = player.getInventory();
    int slot = player.getChosenSlot();
    const ItemStack& stack = inventory->getSlot(slot);
    const auto& def = indices->items.require(stack.getItemId());

    auto& skeleton = *this->skeleton;
    const auto& config = *skeleton.config;
    
    auto itemBone = config.find("item");
    size_t itemBoneIndex = itemBone->getIndex();
    skeleton.modelOverrides.at(itemBoneIndex).model = assets.get<model::Model>(def.modelName);
    skeleton.pose.matrices.at(itemBoneIndex) = matrix;

    // render
    modelBatch.setLightsOffset(camera.position);
    config.update(skeleton, glm::mat4(1.0f), glm::vec3());
    config.render(assets, modelBatch, skeleton, glm::mat4(1.0f), glm::vec3());
}
