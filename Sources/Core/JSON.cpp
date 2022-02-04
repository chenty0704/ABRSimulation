#include "Core/JSON.h"

boost::json::value JSON::Import(std::string_view fileName) {
    std::ifstream stream(fileName.data());
    boost::json::stream_parser parser;
    for (std::string line; std::getline(stream, line); parser.write(line));
    parser.finish();
    return parser.release();
}
