#include "ABRControllers/ModelPredictiveController.h"

DownloadDecision ModelPredictiveController::GetDownloadDecision(const SessionContext &ctx) {
    const auto &opts = dynamic_cast<const ModelPredictiveControllerOptions &>(*ModelPredictiveController::opts);
    DownloadDecision decision{.SegmentID = ctx.NextSegmentID};

    const auto &bitRatesInKbps = ctx.VideoModel.get().EncodingBitRatesInKbps;
    const auto minBitRateInKbps = bitRatesInKbps.front(), maxBitRateInKbps = bitRatesInKbps.back();
    const auto maxBufferLevelInMs =
            static_cast<double>(ctx.MaxBufferSegmentCount) * ctx.VideoModel.get().SegmentDurationInMs;
    const auto targetBufferLevelInMs = maxBufferLevelInMs * opts.TargetBufferRatio;
    const auto throughputEstimateInKbps = ctx.ThroughputEstimator.get().EstimateInKbps();
    const auto lastBitRateInKbps = ctx.BufferedBitRatesInKbps[ctx.NextSegmentID - 1].value();

    static constexpr auto obj =
            "-Sum[-Sqrt[rMin * rMax] * omega * u[k]^2, {k, K}]"
            "+ beta * Sum[((x[k] - xOpt) / (xMax / 2))^2, {k, K}]"
            "+ gamma * (rMin * rMax * (u0 - u[1])^2 + Sum[rMin * rMax * (u[k - 1] - u[k])^2, {k, 2, K}])";
    static constexpr auto cons =
            "Join[{x[1] == x0 - deltaT + omega * deltaT * u[1]}, "
            "Table[x[k] == x[k - 1] - deltaT + omega * deltaT * u[k], {k, 2, K}], "
            "Table[u[k] >= 1 / rMax && u[k] <= 1 / rMin, {k, K}], "
            "Table[x[k] >= 0 && x[k] <= xMax, {k, K}]]";
    static constexpr auto vars = "Join[Table[u[k], {k, K}], Table[x[k], {k, K}]]";
    static const auto body = fmt::format(
            "Quiet[Check["
            "1 / First@QuadraticOptimization[{obj}, {cons}, {vars}, \"PrimalMinimizer\", \"Method\" -> \"Gurobi\"], "
            "omega, QuadraticOptimization::dinfeas], QuadraticOptimization::dinfeas]",
            "obj"_a = obj, "cons"_a = cons, "vars"_a = vars);

    LLU::EvaluateFunction(stream,
                          {"rMin", "rMax", "xMax", "xOpt", "K", "deltaT", "beta", "gamma", "omega", "u0", "x0"}, body,
                          minBitRateInKbps, maxBitRateInKbps, maxBufferLevelInMs, targetBufferLevelInMs,
                          opts.WindowSize, opts.TimeIntervalInMs,
                          opts.BufferDeviationPenaltyFactor, opts.SwitchingCostFactor,
                          throughputEstimateInKbps, 1 / lastBitRateInKbps, ctx.BufferLevelInMs());
    double nextBitRateInKbps;
    stream.get() >> nextBitRateInKbps;
    const auto it = std::upper_bound(bitRatesInKbps.cbegin(), bitRatesInKbps.cend(), nextBitRateInKbps);
    decision.BitRateID = it != bitRatesInKbps.cbegin() ? it - bitRatesInKbps.cbegin() - 1 : 0;

    return decision;
}
