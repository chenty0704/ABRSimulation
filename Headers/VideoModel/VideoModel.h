#pragma once

#include "Core/Core.h"

struct VideoModel {
    double SegmentDurationInMs;
    std::vector<double> BitRatesInKbps;
    std::vector<std::vector<size_t>> SegmentByteCounts;

    static VideoModel Import(std::string_view fileName);

    [[nodiscard]] size_t SegmentCount() const {
        return SegmentByteCounts.size();
    }

    [[nodiscard]] double LengthInMs() const {
        return static_cast<double>(SegmentCount()) * SegmentDurationInMs;
    }
};
