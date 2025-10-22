// clay
#include <clay/application/android/AppAndroid.h>
#include <clay/gui/android/ImGuiComponentAndroid.h>
// application
#include "application/scenes/menu_scene/MenuScene.h"
// class
#include "application/scenes/sandbox_scene/SandboxScene.h"

SandboxScene::SandboxScene(clay::BaseApp& app)
    : clay::BaseScene(app),
      mEntityManager_(app.getGraphicsContext(), app.getResources()) {
    mCamera_.setPosition({0,0,5});
    const auto [frameWidth, frameHeight] = app.getGraphicsContext().getFrameDimensions();
    mCamera_.setAspectRatio(static_cast<float>(frameWidth) / static_cast<float>(frameHeight));

    {
        mSolidSphereEntity_ = mEntityManager_.createEntity();
        clay::ecs::ModelRenderable modelRenderablePlain{};
        modelRenderablePlain.modelHandle = mApp_.getResources().getHandle<clay::Model>("SolidSphere");
        mEntityManager_.addModelRenderable(mSolidSphereEntity_, modelRenderablePlain);
        clay::ecs::Transform transform{};
        transform.mPosition_ = {0, 0, 0};
        mEntityManager_.addTransform(mSolidSphereEntity_, transform);
    }
    {
        mTextureSphereEntity_ = mEntityManager_.createEntity();
        clay::ecs::ModelRenderable modelRenderablePlain{};
        modelRenderablePlain.modelHandle = mApp_.getResources().getHandle<clay::Model>("VTexture");
        mEntityManager_.addModelRenderable(mTextureSphereEntity_, modelRenderablePlain);
        clay::ecs::Transform transform{};
        transform.mPosition_ = {1, 0, 0};
        mEntityManager_.addTransform(mTextureSphereEntity_, transform);
    }
    {
        mTextEntity_ = mEntityManager_.createEntity();
        clay::ecs::TextRenderable text;
        text.initialize(
            mApp_.getGraphicsContext(),
            "HELLO WORLD",
            &mApp_.getResources()[mApp_.getResources().getHandle<clay::Font>("Runescape")]
        );
        text.mScale_ = {.01f,.01f,.01f};
        text.mColor_ = {1,1,0,1};
        mEntityManager_.addTextRenderable(mTextEntity_, text);
        clay::ecs::Transform transform{};
        transform.mPosition_ = {0, 1, 0};
        mEntityManager_.addTransform(mTextEntity_, transform);
    }
}

SandboxScene::~SandboxScene() {}

void SandboxScene::update(const float dt) {
    mEntityManager_.mTransforms[mSolidSphereEntity_].mOrientation_ *= glm::angleAxis(glm::radians(120.0f / float(ImGui::GetIO().Framerate)), glm::vec3(0.0f, 1.0f, 0.0f));
    mEntityManager_.mTransforms[mTextureSphereEntity_].mOrientation_ *= glm::angleAxis(glm::radians(120.0f / float(ImGui::GetIO().Framerate)), glm::vec3(0.0f, 1.0f, 0.0f));
    mEntityManager_.mTransforms[mTextEntity_].mOrientation_ *= glm::angleAxis(glm::radians(120.0f / float(ImGui::GetIO().Framerate)), glm::vec3(0.0f, 1.0f, 0.0f));
}

void SandboxScene::render(vk::CommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);
    // update camera
    clay::BaseScene::CameraConstant ubo{};
    ubo.view = mCamera_.getViewMatrix();
    ubo.proj = mCamera_.getProjectionMatrix();

    ((clay::GraphicsContextAndroid&)(mApp_.getGraphicsContext())).mCameraUniform_->setData(
        &ubo,
        sizeof(CameraConstant)
    );

    mEntityManager_.render(cmdBuffer);
}

void SandboxScene::renderGUI(vk::CommandBuffer cmdBuffer) {
    clay::ImGuiComponentAndroid::beginRender();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Sandbox Scene");

    if (ImGui::Button("Back")) {
        ((clay::AppAndroid&)mApp_).setScene(new MenuScene(mApp_));
    }
    ImGui::End();
    clay::ImGuiComponentAndroid::endRender(cmdBuffer);
}

void SandboxScene::initialize() {}

void SandboxScene::destroyResources() {}
