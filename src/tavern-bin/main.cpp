#include <tavern/tavern.h>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    tavern::tavern engine;

    engine.init();
    engine.run();
    engine.clean();

    return 0;
}
