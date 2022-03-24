#pragma once

#include "Core/Core.h"
#include "VideoModel/VideoModel.h"

struct SessionContext {
    size_t PlaybackTimeInMs = 0;
    std::reference_wrapper<const VideoModel> VideoModel;
    std::span<const std::optional<size_t>> DownloadedBitRateIDs;
};

struct DownloadDecision {
    size_t SegmentID;
    size_t BitRateID;
};

class ABRController {
public:
    virtual DownloadDecision GetDownloadDecision(SessionContext ctx) = 0;

    virtual ~ABRController() = default;

protected:
    size_t nextSegmentID = 1;
};
