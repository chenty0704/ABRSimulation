#pragma once

#include <LLU/LLU.h>

#include "ABRSimulation/ABRSimulation.h"
#include "Interface/CoreInterface.h"
#include "Interface/ThroughputEstimatorsInterface.h"
#include "Interface/ABRControllersInterface.h"

namespace LLU {
    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator>>(WSStream<EIn, EOut> &stream, SessionOptions &opts) {
        WS::List rules;
        stream >> rules;
        for (auto i = 0; i < rules.getArgc(); ++i) {
            std::string key;
            stream >> WS::Rule >> key;
            if (key == "MaxBufferSegmentCount") stream >> opts.MaxBufferSegmentCount;
            else std::abort();
        }
        return stream;
    }

    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator<<(WSStream<EIn, EOut> &stream, const SimulationData &simData) {
        return stream << WS::Association(6)
                      << WS::Rule << "TotalTimeInMs" << simData.TotalTimeInMs
                      << WS::Rule << "BufferedBitRatesInKbps" << simData.BufferedBitRatesInKbps
                      << WS::Rule << "RebufferingDurationsInMs" << simData.RebufferingDurationsInMs
                      << WS::Rule << "FullBufferDelaysInMs" << simData.FullBufferDelaysInMs
                      << WS::Rule << "DownloadDurationsInMs" << simData.DownloadDurationsInMs
                      << WS::Rule << "DownloadBitRatesInKbps" << simData.DownloadBitRatesInKbps;
    }
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
