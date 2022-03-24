#include "Core/JSON.h"
#include "VideoModel/VideoModel.h"

VideoModel VideoModel::Import(std::string_view fileName) {
    const auto obj = JSON::Import(fileName).as_object();
    return {
            .SegmentDurationInMs = boost::json::value_to<size_t>(obj.at("SegmentDurationInMs")),
            .BitRatesInKbps = boost::json::value_to<std::vector<size_t>>(obj.at("BitRatesInKbps")),
            .SegmentByteCounts = boost::json::value_to<std::vector<std::vector<size_t>>>(obj.at("SegmentByteCounts"))
    };
}
