#include "App.hpp"

#include <SFML/GpuPreference.hpp>

SFML_DEFINE_DISCRETE_GPU_PREFERENCE

int main(int argc, char* argv[])
{
    (void)(argc);
    (void)(argv);

    App app;
    app.run();

    return 0;
}
