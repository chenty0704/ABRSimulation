#pragma once

#include <LLU/LLU.h>

#include "ABRSimulation/ABRSimulation.h"
#include "Interface/CoreInterface.h"
#include "Interface/ThroughputEstimatorsInterface.h"
#include "Interface/ABRControllersInterface.h"

namespace LLU {
    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator>>(WSStream<EIn, EOut> &stream, SessionOptions &opts) {
        WS::Association assoc;
        stream >> assoc;
        for (auto i = 0; i < assoc.getArgc(); ++i) {
            std::string key;
            stream >> WS::Rule >> key;
            if (key == "MaxBufferSegmentCount") stream >> opts.MaxBufferSegmentCount;
            else std::abort();
        }
        return stream;
    }

    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator<<(WSStream<EIn, EOut> &stream, const SimulationData &simData) {
        return stream << WS::Association(4)
                      << WS::Rule << "TotalTimeInMs" << simData.TotalTimeInMs
                      << WS::Rule << "DownloadedBitRatesInKbps" << simData.DownloadedBitRatesInKbps
                      << WS::Rule << "RebufferingDurationsInMs" << simData.RebufferingDurationsInMs
                      << WS::Rule << "FullBufferDelaysInMs" << simData.FullBufferDelaysInMs;
    }
}

/// \param videoModelFile:String
/// \param networkModelFile:String
/// \param controllerType:String
/// \param controllerOptions:Association
/// \param throughputEstimatorType:String
/// \param throughputEstimatorOptions:Association
/// \param sessionOptions:Association
/// \return Association
extern "C" DLLEXPORT int ABRSessionSimulate(WolframLibraryData libData, WSLINK wslink);
