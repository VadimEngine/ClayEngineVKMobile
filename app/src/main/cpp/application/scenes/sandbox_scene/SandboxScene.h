#pragma once
// clay
#include <clay/application/common/BaseScene.h>
#include <clay/ecs/EntityManager.h>

class SandboxScene : public clay::BaseScene {
public:
    SandboxScene(clay::BaseApp& app);

    ~SandboxScene();

    void update(const float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

    clay::ecs::EntityManager mEntityManager_;

    clay::ecs::Entity mSolidSphereEntity_;
    clay::ecs::Entity mTextureSphereEntity_;
    clay::ecs::Entity mTextEntity_;
};


