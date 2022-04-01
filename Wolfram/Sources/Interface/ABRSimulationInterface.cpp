#include "VideoModel/VideoModel.h"
#include "NetworkModel/NetworkModel.h"
#include "Interface/ABRSimulationInterface.h"

int ABRSessionSimulate(WolframLibraryData, WSLINK wslink) {
    try {
        LLU::WSStream<LLU::WS::Encoding::Native> stream(wslink);

        std::string videoModelFile, networkModelFile;
        std::unique_ptr<ABRController> controller;
        std::unique_ptr<ThroughputEstimator> throughputEstimator;
        SessionOptions opts; // NOLINT(cppcoreguidelines-pro-type-member-init)

        stream >> LLU::WS::List(7) >> videoModelFile >> networkModelFile >> controller >> throughputEstimator >> opts;
        const auto videoModel = VideoModel::Import(videoModelFile);
        auto networkModel = NetworkModel::Import(networkModelFile);

        stream << ABRSimulation::SimulateSession(videoModel, networkModel, *controller, *throughputEstimator, opts);
    } catch (const LLU::LibraryLinkError &e) {
        return e.which();
    } catch (...) {
        return LLU::ErrorCode::FunctionError;
    }
    return LLU::ErrorCode::NoError;
}
