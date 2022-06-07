#pragma once

#include "ABRSimulation/ABRSimulation.h"
#include "Links/CoreLink.h"

namespace LLU {
    LLU_REGISTER_INPUT_TYPE(SessionOptions,
                            MaxBufferSegmentCount)

    LLU_REGISTER_OUTPUT_TYPE(SimulationData,
                             TotalTimeInMs,
                             MaxBufferLevelInMs,
                             BufferedBitRatesInKbps,
                             DownloadDurationsInMs, DownloadBitRatesInKbps,
                             BufferTimesInMs, BufferLevelsInMs,
                             RebufferingPeriodsInMs,
                             FullBufferDelaysInMs)
}

/// \param videoModel:_VideoModel
/// \param netModel:_NetworkModel
/// \param controllerType:_String
/// \param controllerOpts:OptionsPattern[]
/// \param throughputEstimatorType:_String
/// \param throughputEstimatorOpts:OptionsPattern[]
/// \param sessionOpts:OptionsPattern[]
/// \return Association
extern "C" DLLEXPORT int ABRSessionSimulate(WolframLibraryData libData, WSLINK link);
