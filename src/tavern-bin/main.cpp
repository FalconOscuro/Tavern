#include <tavern/tavern.h>

int main(int argc, char** argv) {
    tavern::tavern engine;

    engine.init();
    engine.run();
    engine.clean();

    return 0;
}
