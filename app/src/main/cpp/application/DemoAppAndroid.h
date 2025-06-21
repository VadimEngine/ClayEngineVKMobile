#pragma once
// clay
#include <clay/application/android/AppAndroid.h>

class DemoAppAndroid :  public clay::AppAndroid {
public:
    DemoAppAndroid(android_app* app);

    ~DemoAppAndroid();

    void loadResources() override;
};
