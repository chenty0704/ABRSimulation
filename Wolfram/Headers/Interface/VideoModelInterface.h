#pragma once

#include "VideoModel/VideoModel.h"
#include "Interface/CoreInterface.h"

namespace LLU {
    LLU_REGISTER_INPUT_TYPE(VideoModel,
                            SegmentDurationInMs,
                            BitRatesInKbps,
                            SegmentByteCounts)
}
