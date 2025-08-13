# Tavern
A WIP 3D VTT, with a custom ECS based on entt and domain specific language cantrip using C++ 17.

# Building
Ensure all submodules are up to date before building using:
```
git submodule update --init --recursive
```

The project is build using CMake 3.30, with presets for `debug`, `release`, `relWithDebInfo` and `minSizeRel` using the following commands from the project root directory:
```
cmake --preset PRESET_NAME
cmake --build --preset PRESET_NAME
```

Development primarily used gcc and clang, with clang being the recommended compiler, take caution with using MSVC as it is untested and may run into some issues with the ecs type-name resolution which uses compile time macros and templating.
