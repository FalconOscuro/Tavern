#include "tavern/resource/mesh_manager.h"

#include <boost/log/trivial.hpp>


namespace tavern::resource {

graphics::mesh* mesh_manager::load_new(file::ifile* file)
{
    constexpr auto MIN_SIZE = 2 * sizeof(size_t);
    const size_t file_size = file->size();

    // must be greater size than header
    if (file_size < MIN_SIZE) {
        BOOST_LOG_TRIVIAL(error) << file->get_path()
            << ": Expected minimum file size of (" << MIN_SIZE << ") "
               "but got (" << file_size << ')';

        return nullptr;
    }

    size_t num_vertices, num_faces;

    file->get_data(&num_vertices);
    file->get_data(&num_faces);

    const size_t expected_size = MIN_SIZE + (num_vertices * sizeof(graphics::vertex)) + (num_faces * sizeof(graphics::face));

    // no faces/vertices or file not big enough to support data size
    if (!(num_faces && num_vertices)) {
        BOOST_LOG_TRIVIAL(error) << file->get_path()
            << ": Vertex/Face count cannot be 0, "
               "num_faces(" << num_faces << "), "
               "num_vertices(" << num_vertices << ')';

        return nullptr;
    }

    else if (file_size < expected_size)
    {
        BOOST_LOG_TRIVIAL(error) << file->get_path()
            << ": File size was smaller than vertex/face count indicated"
               ", num_faces(" << num_faces << "), "
               "num_vertices(" << num_vertices << "), "
               "expected_size(" << expected_size << "), "
               "actual_size(" << file_size << ')';

        return nullptr;
    }

    // read vertices
    graphics::vertex* vertices = new graphics::vertex[num_vertices];
    size_t vertices_read = file->get_data(vertices, num_vertices);
    if (vertices_read != num_vertices)
    {
        BOOST_LOG_TRIVIAL(error) << file->get_path()
            << ": Failed to read all vertices, "
               "expected " << num_vertices
            << " but got " << vertices_read;

        delete[] vertices;
        return nullptr;
    }

    // read faces
    graphics::face* faces = new graphics::face[num_faces];
    size_t faces_read = file->get_data(faces, num_faces);
    if (faces_read != num_faces)
    {
        BOOST_LOG_TRIVIAL(error) << file->get_path()
            << ": Failed to read all faces, "
               "expected " << num_faces
            << " but got " << faces_read;

        delete[] vertices;
        delete[] faces;
        return nullptr;
    }

    graphics::mesh* mesh = new graphics::mesh(vertices, faces, num_vertices, num_faces);

    delete[] vertices;
    delete[] faces;
    return mesh;
}

} /* end of namespace tavern::resource */
