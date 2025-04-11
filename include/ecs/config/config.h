#ifndef ECS_CONFIG_H
#define ECS_CONFIG_H

#if defined(_MSVC_VER)
    #define __FUNC_PRETTY__ __FUNCSIG__
    #define FUNC_PRETTY_PREFIX '<'

#elif defined(__GNUC__) || defined(__clang__)
    #define __FUNC_PRETTY__ __PRETTY_FUNCTION__
    #define FUNC_PRETTY_PREFIX '='
    #define FUNC_PRETTY_END ';'

#else
    #define __FUNC_PRETTY__ ""
    #define FUNC_PRETTY_PREFIX ' '

#endif

#endif /* end of include guard: ECS_CONFIG_H */
