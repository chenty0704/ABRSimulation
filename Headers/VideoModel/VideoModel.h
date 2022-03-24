#pragma once

#include "Core/Core.h"

struct VideoModel {
    size_t SegmentDurationInMs;
    std::vector<size_t> BitRatesInKbps;
    std::vector<std::vector<size_t>> SegmentByteCounts;

    static VideoModel Import(std::string_view fileName);
};
