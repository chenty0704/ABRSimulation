#pragma once

#include "VideoModel/VideoModel.h"
#include "Links/CoreLink.h"

namespace LLU {
    LLU_REGISTER_INPUT_TYPE(VideoModel,
                            SegmentDurationInMs,
                            EncodingBitRatesInKbps,
                            SegmentByteCounts)
}
