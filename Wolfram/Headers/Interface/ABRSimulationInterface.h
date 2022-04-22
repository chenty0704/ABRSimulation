#pragma once

#include "ABRSimulation/ABRSimulation.h"
#include "Interface/CoreInterface.h"

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

/// \param videoModelFile:String
/// \param networkModelFile:String
/// \param controllerType:String
/// \param controllerOptions:List[Rule, ...]
/// \param throughputEstimatorType:String
/// \param throughputEstimatorOptions:List[Rule, ...]
/// \param sessionOptions:List[Rule, ...]
/// \return Association
extern "C" DLLEXPORT int ABRSessionSimulate(WolframLibraryData libData, WSLINK wslink);
