#pragma once
// clay
#include <clay/application/common/BaseScene.h>

class SandboxScene : public clay::BaseScene {
public:
    SandboxScene(clay::BaseApp& app);

    ~SandboxScene();

    void update(const float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

    std::vector<std::unique_ptr<clay::Entity>> mEntities_;
};


