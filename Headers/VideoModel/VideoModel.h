#pragma once

#include "Core/Core.h"

struct VideoModel {
    int SegmentDurationInMs;
    std::vector<int> BitRatesInKbps;
    std::vector<std::vector<int>> SegmentBitCounts;
};
