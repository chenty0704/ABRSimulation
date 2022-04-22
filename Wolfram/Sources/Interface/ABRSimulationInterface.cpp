#include "Interface/VideoModelInterface.h"
#include "Interface/NetworkModelInterface.h"
#include "Interface/ThroughputEstimatorsInterface.h"
#include "Interface/ABRControllersInterface.h"
#include "Interface/ABRSimulationInterface.h"

int ABRSessionSimulate(WolframLibraryData, WSLINK wslink) {
    try {
        LLU::NativeWSStream stream(wslink);

        VideoModel videoModel;
        NetworkModel networkModel;
        std::unique_ptr<IABRController> controller;
        std::unique_ptr<IThroughputEstimator> throughputEstimator;
        SessionOptions opts;

        stream >> LLU::WS::List(7) >> videoModel >> networkModel >> controller >> throughputEstimator >> opts;
        stream << ABRSimulation::SimulateSession(videoModel, networkModel, *controller, *throughputEstimator, opts);
    } catch (const LLU::LibraryLinkError &e) {
        return e.which();
    } catch (...) {
        return LLU::ErrorCode::FunctionError;
    }
    return LLU::ErrorCode::NoError;
}
