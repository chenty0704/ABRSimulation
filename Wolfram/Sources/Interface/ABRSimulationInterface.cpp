#include "VideoModel/VideoModel.h"
#include "NetworkModel/NetworkModel.h"
#include "Interface/ABRSimulationInterface.h"

int ABRSessionSimulate(WolframLibraryData, WSLINK wslink) {
    try {
        LLU::NativeWSStream stream(wslink);

        std::string videoModelFile, networkModelFile;
        std::unique_ptr<IABRController> controller;
        std::unique_ptr<IThroughputEstimator> throughputEstimator;
        SessionOptions opts;

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
