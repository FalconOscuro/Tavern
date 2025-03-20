#include <tavern/tavern.h>

#include <iostream>

#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include "tavern-bin/panels/performance.h"
#include "tavern-bin/panels/scene.h"

int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description opts("Tavern Usage");
    opts.add_options()
        ("help,h", "display help message")
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

    {
        tavern::engine& engine = tavern::engine::singleton();

        if (!engine.init(
                args["width"].as<uint16_t>(),
                args["height"].as<uint16_t>(),
                args["title"].as<std::string>()
        )) {
            BOOST_LOG_TRIVIAL(error) << "Initialization failed!";
            return 1;
        }

        //if (args.count("scene")) {
        //    const std::string scene_file = args["scene"].as<std::string>();

        //    engine.get_scene().load_scene(scene_file);
        //}
        
        tavern::graphics::renderer::singleton().add_gui_layer("Performance", new panel::performance());
        tavern::graphics::renderer::singleton().add_gui_layer("Scene", new panel::scene_p());

        engine.run();
        engine.shutdown();
    }

    BOOST_LOG_TRIVIAL(trace) << "Program terminating";

    return 0;
}
