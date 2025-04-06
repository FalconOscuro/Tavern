#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <tavern/assets/pipeline.h>
#include <boost/program_options/parsers.hpp>

int main(int argc, char** argv)
{
    std::string file_name;

    namespace po = boost::program_options;
    po::options_description opts("Tavern Asset Pipeline Usage");
    opts.add_options()
        ("help,h", "display help message")
        ("file,f", po::value<std::string>(), "file to convert")
    ;


    po::variables_map args;
    try {
        po::store(
            po::command_line_parser(argc, argv).options(opts).run(),
            args
        );
    }
    catch (const po::error& e) {
        std::cerr << "Failed to parse command-line args: " << e.what();
    }

    if (args.count("help")) {
        opts.print(std::cout);
        return 0;
    }

    else if (args.count("file") && !args["file"].empty())
        file_name = args["file"].as<std::string>();

    else while (file_name.empty())
    {
        std::cout << "Path to file being converted: ";
        std::cin >> file_name;
    }

    return tavern::assets::convert_file(file_name) ? 0 : 1;
}
