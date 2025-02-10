#include "tavern/resource/mesh_manager.h"

#include <boost/log/trivial.hpp>

namespace tavern::resource {

mesh_manager::resource_ptr mesh_manager::load_new(const std::string& path)
{
    (void)path;
    BOOST_LOG_TRIVIAL(error) << "Direct loading model mesh from file is no longer supported";
    return nullptr;
}

} /* end of namespace tavern::resource */
