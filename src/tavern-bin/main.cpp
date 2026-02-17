#include <tavern/tavern.h>
#include <file/tpk/tpk_package.h>

#include <iostream>

#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include "tavern-bin/panels/cantrip.h"
#include "tavern-bin/panels/file_system.h"
#include "tavern-bin/panels/performance.h"
#include "tavern-bin/panels/scene.h"

#include <tavern/components/camera.h>
#include <tavern/components/transform.h>

int make_tpk()
{
    std::string in, out, name, author;

    std::cout << "Target directory: ";
    std::cin >> in;

    std::cout << "Output file: ";
    std::cin >> out;

    std::cout << "TPK name: ";
    std::cin >> name;

    std::cout << "Author: ";
    std::cin >> author;

    return tavern::file::tpk::package_directory(in, out, name, author, 1) ? 0 : 1;
}

int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description opts("Tavern Usage");
    opts.add_options()
        ("help,h", "display help message")
        ("tpk", "write directory as tpk")
        ("pcomp", "print component types")
        ("scene", po::value<std::string>(), "load scene from file")
        ("title", po::value<std::string>()->default_value("Tavern"), "set window title")
        ("width", po::value<uint16_t>()->default_value(1920), "window width")
        ("height", po::value<uint16_t>()->default_value(1080), "window height")
    ;

    po::variables_map args;
    try {
        po::store(
            po::command_line_parser(argc, argv).options(opts).run(),
            args
        );
    }
    catch (const po::error& e) {
        BOOST_LOG_TRIVIAL(error) << "Failed to parse command-line args: " << e.what();
    }

    po::notify(args);

    if (args.count("help")) {
        opts.print(std::cout);
        return 0;
    }

    else if (args.count("tpk"))
        return make_tpk();

    else if (args.count("pcomp"))
    {
        BOOST_LOG_TRIVIAL(trace) << ecs::core::internal::get_type_name<tavern::component::camera>();
        BOOST_LOG_TRIVIAL(trace) << ecs::core::internal::get_type_name<tavern::component::transform>();
        return 0;
    }

    tavern::engine& engine = tavern::engine::singleton();

    if (!engine.init(
            args["width"].as<uint16_t>(),
            args["height"].as<uint16_t>(),
            args["title"].as<std::string>()
    )) {
        BOOST_LOG_TRIVIAL(error) << "Initialization failed!";
        return 1;
    }

    if (args.count("scene")) {
        const std::string scene_file = args["scene"].as<std::string>();

        tavern::scene::singleton().load(scene_file);
    }
    
    {
        auto& renderer = tavern::graphics::renderer::singleton();

        renderer.add_gui_layer("Cantrip", new panel::cantrip_panel());
        renderer.add_gui_layer("File System", new panel::file_system_p());
        renderer.add_gui_layer("Performance", new panel::performance());
        renderer.add_gui_layer("Scene", new panel::scene_p());
    }

    engine.run();
    engine.shutdown();

    BOOST_LOG_TRIVIAL(trace) << "Program terminating";

    return 0;
}
