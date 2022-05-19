#include "Links/VideoModelLink.h"
#include "Links/NetworkModelLink.h"
#include "Links/ThroughputEstimatorsLink.h"
#include "Links/ABRControllersLink.h"
#include "Links/ABRSimulationLink.h"

int ABRSessionSimulate(WolframLibraryData, WSLINK wslink) {
    try {
        LLU::WSNativeStream stream(wslink);

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
