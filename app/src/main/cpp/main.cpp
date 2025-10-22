// third party
#include <android/log.h>
#include <android_native_app_glue.h>
#include "imgui_impl_android.h"
#include <PxPhysicsAPI.h>
// clay
#include "clay/utils/common/Logger.h"
#include "application/DemoAppAndroid.h"
#include "application/scenes/menu_scene/MenuScene.h"

using namespace physx;

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

    bool once = false;

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
            if (!once) {
                // Initialize PhysX Foundation and Physics
                static PxDefaultAllocator gAllocator;
                static PxDefaultErrorCallback gErrorCallback;

                PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator
                                                              , gErrorCallback
                );
                if (!foundation) {
                    LOG_E("PhysX Foundation creation failed");
                    //printf("PhysX Foundation creation failed!\n");
                    //return -1;
                }

                PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation
                                                     , PxTolerancesScale());
                if (!physics) {
                    LOG_E("PhysX Physics creation failed");
                    //printf("PhysX Physics creation failed!\n");
                    foundation->release();
                    //return -1;
                }

                // Create a simple scene
                PxSceneDesc sceneDesc(physics->getTolerancesScale());
                sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
                sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
                sceneDesc.filterShader = PxDefaultSimulationFilterShader;

                PxScene* scene = physics->createScene(sceneDesc);
                if (!scene) {
                    LOG_E("PhysX Scene creation failed");
                    //printf("PhysX Scene creation failed!\n");
                    physics->release();
                    foundation->release();
                    //return -1;
                }
                LOG_I("PhysX initialized successfully");
                //printf("PhysX initialized successfully!\n");

                // Cleanup
                scene->release();
                physics->release();
                foundation->release();

                once = true;
            }

            demoApp->update();
            demoApp->render();
        }
    }
}