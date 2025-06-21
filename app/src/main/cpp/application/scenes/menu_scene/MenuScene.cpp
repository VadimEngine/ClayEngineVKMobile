// clay
#include <clay/application/android/AppAndroid.h>
#include <clay/gui/android/ImGuiComponentAndroid.h>
// application
#include "application/scenes/sandbox_scene/SandboxScene.h"
// class
#include "application/scenes/menu_scene/MenuScene.h"

MenuScene::MenuScene(clay::BaseApp& app)
    : clay::BaseScene(app) {}

MenuScene::~MenuScene() {}

void MenuScene::update(const float dt) {}

void MenuScene::render(VkCommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);
}

void MenuScene::renderGUI(VkCommandBuffer cmdBuffer) {
    clay::ImGuiComponentAndroid::beginRender();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Clay EngineVK Sandbox");

    if (ImGui::Button("Sandbox Scene")) {
        ((clay::AppAndroid&)mApp_).setScene(new SandboxScene(mApp_));
    }
    ImGui::End();
    clay::ImGuiComponentAndroid::endRender(cmdBuffer);
}

void MenuScene::initialize() {}

void MenuScene::destroyResources() {}
