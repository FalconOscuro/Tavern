#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include "c4/yml/parse.hpp"
#include "resource_type_manager.hpp"

#include <boost/log/trivial.hpp>

#include <ryml_std.hpp>
#include <ryml.hpp>

#include "../graphics/graphics.h"
#include "../util/read_file.hpp"

namespace tavern::resource {

class shader_manager : public resource_type_manager<graphics::shader>
{
public:
    typedef resource_type_manager<graphics::shader> base;

    shader_manager():
        base()
    {}

protected:

    graphics::shader* load_new(const std::string& path) override
    {
        uint32_t size;   
        char* raw = utility::read_file(path.c_str(), size);

        if (raw == nullptr) {
            BOOST_LOG_TRIVIAL(warning) << "Failed to open shader config file: " << path;
            return nullptr;
        }

        ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(raw));

        std::string vertex_path, frag_path;

        tree["vertex"] >> vertex_path;
        tree["fragment"] >> frag_path;

        delete[] raw;

        char* vertex_raw = utility::read_file(vertex_path.c_str(), size);
        if (vertex_raw == nullptr) {
            BOOST_LOG_TRIVIAL(warning) << "Failed to open vertex shader file: " << vertex_path;
            return nullptr;
        }

        char* frag_raw   = utility::read_file(frag_path.c_str()  , size);
        if (frag_raw == nullptr) {
            delete[] vertex_raw;
            BOOST_LOG_TRIVIAL(warning) << "Failed to open fragment shader file: " << vertex_path;
            return nullptr;
        }

        graphics::shader* shader = new graphics::shader(vertex_raw, frag_raw);
        delete[] vertex_raw;
        delete[] frag_raw;

        return shader;
    }

}; /* end of class shader_manager : public resource_type_manager<graphics::shader> */

} /* end of namespace tavern::resource */

#endif /* end of define guard SHADER_MANAGER_HPP */
