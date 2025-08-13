#include <cantrip/scanner/scanner.hpp>
#include <cantrip/parser/parser.h>
#include <cantrip/analyzer/semantic.h>

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <file/physical_file.h>

int main(int argc, char** argv)
{
    namespace po = boost::program_options;

    po::options_description desc("Cantrip Usage");
    desc.add_options()
        ("help,h", "display help message")
        ("verbose,v", "print extra information")
        ("file", po::value<std::vector<std::string>>(), "input cantrip file(s)")
    ;

    po::positional_options_description pos_desc;
    pos_desc.add("file", -1);

    po::variables_map args;

    try {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), args);
    }
    catch (const po::error& e) {
        BOOST_LOG_TRIVIAL(error) << "Failed to parse command-line args: " << e.what();
    }

    po::notify(args);

    if (args.count("help")) {
        desc.print(std::cout);
        return 0;
    }

    const bool verbose = args.count("verbose");

    std::vector<std::string> input_files = args["file"].as<std::vector<std::string>>();

    if (input_files.empty())
    {
        BOOST_LOG_TRIVIAL(error) << "Must specify at least one cantrip input file.";
        return -1;
    }

    std::vector<tavern::file::file_handle> files;
    files.reserve(input_files.size());

    for (auto it = input_files.begin(); it != input_files.end(); ++it)
        files.emplace_back(std::make_unique<tavern::file::physical_file>(*it));

    const auto scan_start = std::chrono::high_resolution_clock::now();
    std::vector<cantrip::token> tokens;

    {
        cantrip::scanner scanner = cantrip::scanner(std::move(files));

        if (!scanner.open())
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to open file";
            return -1;
        }

        do {
            tokens.emplace_back(scanner.peek());
            scanner.pop();

            if (tokens.back() == cantrip::ERROR)
            {
                BOOST_LOG_TRIVIAL(error) << "Scanner error: " << tokens.back().to_string();
                return -1;
            }

        } while(tokens.back() != cantrip::MODULE_END);
    }

    auto scan_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - scan_start);

    if (verbose) for (size_t i = 0; i < tokens.size(); ++i)
        std::cout << '[' << i << "]: " << tokens[i].to_string() << '\n';

    std::cout << "Scan time: " << scan_time.count() / 1000.f << " ms\n";
    std::cout << "Tokens scanned: " << tokens.size() << '\n';

    const auto parse_start = std::chrono::high_resolution_clock::now();

    cantrip::module module;
    try
    {
        cantrip::parser parser = cantrip::parser(std::move(tokens));

        parser.parse_module(module);
    }
    catch (cantrip::error::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << e.what();
        return 1;
    }

    const auto parse_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - parse_start);

    std::cout << "Parse time: " << parse_time.count() / 1000.f << " ms\n";

    if (verbose)
    {
        std::cout << "Found (" << module.components.size() << ") components:\n";
        for (auto it = module.components.begin(); it != module.components.end(); ++it)
            std::cout << it->first << '\n';

        std::cout << "Found (" << module.functions.size() << ") functions:\n";
        for (auto it = module.functions.begin(); it != module.functions.end(); ++it)
            std::cout << it->first << '\n';

        std::cout << "Found (" << module.systems.size() << ") systems:\n";
        for (auto it = module.systems.begin(); it != module.systems.end(); ++it)
            std::cout << it->first << '\n';
    }

    const auto analysis_start = std::chrono::high_resolution_clock::now();

    try {
        cantrip::analyzer::semantic semantic_analyzer;
        semantic_analyzer.analyze_module(&module);
    }
    catch (cantrip::error::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Semantic analysis failed: " << e.what();
        return 1;
    }

    const auto analysis_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - analysis_start);

    std::cout << "Semantic analysis time: " << analysis_time.count() / 1000.f << std::endl;
}
