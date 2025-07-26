#pragma once

#include <memory>

class Assets;
class Camera;
class Level;
class Player;
class ModelBatch;

namespace rigging {
    struct Skeleton;
}

class HandsRenderer {
public:
    HandsRenderer(
        const Assets& assets,
        const Level& level,
        const Player& player,
        ModelBatch& modelBatch,
        std::shared_ptr<rigging::Skeleton> skeleton
    );

    void renderHands(const Camera& camera, float delta);
private:
    const Assets& assets;
    const Level& level;
    const Player& player;
    ModelBatch& modelBatch;
    std::shared_ptr<rigging::Skeleton> skeleton;
};
