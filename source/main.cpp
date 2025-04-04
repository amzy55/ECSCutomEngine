#include <iostream>

#include "core/engine.hpp"

using namespace bee;

int main()
{
    Engine.Initialize();
    Engine.Run();
    Engine.Shutdown();
}
