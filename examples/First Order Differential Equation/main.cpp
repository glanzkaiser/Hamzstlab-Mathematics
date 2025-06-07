//Modified from:  demo.cpp (implot-demos) by Evan Pezent (evanpezent.com)

#include "App.h"

struct ImPlotDemo : App {
    using App::App;
    void Update() override {
        ImPlot::ShowFirstOrderDEWindow();   
    }
};

int main(int argc, char const *argv[])
{
    ImPlotDemo app("Hamzstlab Mathematics",1920,1080,argc,argv);
    app.Run();

    return 0;
}
