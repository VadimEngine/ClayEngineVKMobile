// third party
#include <android/log.h>
#include <android_native_app_glue.h>
#include "imgui_impl_android.h"
// clay
#include "clay/utils/common/Logger.h"
#include "application/DemoAppAndroid.h"
#include "application/scenes/menu_scene/MenuScene.h"


static int32_t handleInputEvent(struct android_app* app, AInputEvent* inputEvent) {
    if (ImGui_ImplAndroid_HandleInputEvent(inputEvent)) {
        return 1; // ImGui handled it
    } else {
        return 0;
    }
}

static DemoAppAndroid* demoApp = nullptr;

void handle_cmd(android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (app->window != nullptr) {
                LOG_I("Window initialized.");
                demoApp = new DemoAppAndroid(app);
                clay::Resources::setFileLoader(
                    [](const std::string& path) {
                        return demoApp->loadFileToMemory(path);
                    }
                );
                demoApp->loadResources();
                demoApp->setScene(new MenuScene(*demoApp));
            }
            break;
        case APP_CMD_TERM_WINDOW:
            LOG_I("Window terminated.");
            break;
    }
}

void android_main(android_app* app) {
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handleInputEvent;

    // Attach the thread to the JVM
    JNIEnv* env;
    app->activity->vm->AttachCurrentThread(&env, nullptr);

    LOG_I("NDK app started.");

    // Main event loop
    int events;
    android_poll_source* source;

    while (true) {
        while (ALooper_pollAll(0, nullptr, &events, (void**)&source) >= 0) {
            if (source) {
                source->process(app, source);
            }

            if (app->destroyRequested != 0) {
                LOG_I("Destroy requested, exiting.");
                return;
            }
        }

        if (demoApp != nullptr) {
            demoApp->update();
            demoApp->render();
        }
    }
}