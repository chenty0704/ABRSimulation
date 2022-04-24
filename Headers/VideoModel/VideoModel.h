#pragma once

#include "Core/Core.h"

struct VideoModel {
    double SegmentDurationInMs;
    std::vector<double> EncodingBitRatesInKbps;
    std::vector<std::vector<size_t>> SegmentByteCounts;

    [[nodiscard]] size_t SegmentCount() const {
        return SegmentByteCounts.size();
    }

    [[nodiscard]] double LengthInMs() const {
        return static_cast<double>(SegmentCount()) * SegmentDurationInMs;
    }
};
