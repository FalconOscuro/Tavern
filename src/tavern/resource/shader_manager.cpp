#include "tavern/resource/shader_manager.hpp"

#include <boost/log/trivial.hpp>

#include <ryml.hpp>
#include <ryml_std.hpp>

#include "tavern/resource/util/file.hpp"

namespace tavern::resource {

graphics::shader* shader_manager::load_new(const std::string& path)
{
    size_t size;
    char* raw = utility::read_file(path.c_str(), size);

    if (raw == nullptr) {
        BOOST_LOG_TRIVIAL(warning) << "Failed to open shader config file: " << path;
        return nullptr;
    }

    ryml::Tree tree = ryml::parse_in_place(raw);

    std::string vertex_path, frag_path;

    tree["vertex"] >> vertex_path;
    tree["fragment"] >> frag_path;

    delete[] raw;

    std::string path_dir = std::string(utility::get_file_parent_dir(path));

    char* vertex_raw = utility::read_file((path_dir + vertex_path).c_str(), size);
    if (vertex_raw == nullptr) {
        BOOST_LOG_TRIVIAL(warning) << "Failed to open vertex shader file: " << vertex_path;
        return nullptr;
    }

    char* frag_raw   = utility::read_file((path_dir + frag_path).c_str()  , size);
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

} /* end of namespace tavern::resource */
