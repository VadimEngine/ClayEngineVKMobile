// clay
#include <clay/application/android/AppAndroid.h>
#include <clay/gui/android/ImGuiComponentAndroid.h>
#include <clay/entity/render/ModelRenderable.h>
#include <clay/entity/render/TextRenderable.h>
// application
#include "application/scenes/menu_scene/MenuScene.h"
// class
#include "application/scenes/sandbox_scene/SandboxScene.h"

SandboxScene::SandboxScene(clay::BaseApp& app)
    : clay::BaseScene(app) {
    mCamera_.setPosition({0,0,5});
    const auto [frameWidth, frameHeight] = app.getGraphicsContext().getFrameDimensions();
    mCamera_.setAspectRatio(static_cast<float>(frameWidth) / static_cast<float>(frameHeight));

    auto* entity1 = new clay::Entity();
    auto* modelRenderable = new clay::ModelRenderable(mApp_.getResources().getResource<clay::Model>("SolidSphere"));
    entity1->addRenderable(modelRenderable);
    entity1->setPosition({0,0,0});
    mEntities_.emplace_back(entity1);

    auto* entity2 = new clay::Entity();
    auto* modelRenderable2= new clay::ModelRenderable(mApp_.getResources().getResource<clay::Model>("VTexture"));
    entity2->addRenderable(modelRenderable2);
    entity2->setPosition({1,0,0});
    mEntities_.emplace_back(entity2);

    auto* entity3 = new clay::Entity();
    auto* textRenderable = new clay::TextRenderable(mApp_.getGraphicsContext(), "HELLO WORLD", mApp_.getResources().getResource<clay::Font>("Runescape"));
    textRenderable->setScale({.01f,.01f,.01f});
    textRenderable->setColor({1,1,0,1});
    entity3->addRenderable(textRenderable);
    mEntities_.emplace_back(entity3);
}

SandboxScene::~SandboxScene() {}

void SandboxScene::update(const float dt) {
    mEntities_[1]->getOrientation() *= glm::angleAxis(glm::radians(120.0f / float(ImGui::GetIO().Framerate)), glm::vec3(0.0f, 1.0f, 0.0f));
}

void SandboxScene::render(VkCommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);
    // update camera
    clay::BaseScene::CameraConstant ubo{};
    ubo.view = mCamera_.getViewMatrix();
    ubo.proj = mCamera_.getProjectionMatrix();

    ((clay::GraphicsContextAndroid&)(mApp_.getGraphicsContext())).mCameraUniform_->setData(
        &ubo,
        sizeof(CameraConstant)
    );

    for (auto& eachEntity : mEntities_) {
        eachEntity->render(cmdBuffer);
    }
}

void SandboxScene::renderGUI(VkCommandBuffer cmdBuffer) {
    clay::ImGuiComponentAndroid::beginRender();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Sandbox Scene");

    if (ImGui::Button("Back")) {
        // TODO set back to menu scene
        ((clay::AppAndroid&)mApp_).setScene(new MenuScene(mApp_));
    }
    ImGui::End();
    clay::ImGuiComponentAndroid::endRender(cmdBuffer);
}

void SandboxScene::initialize() {}

void SandboxScene::destroyResources() {}
