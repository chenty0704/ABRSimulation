#include "ABRControllers/ModelPredictiveController.h"

DownloadDecision ModelPredictiveController::GetDownloadDecision(const SessionContext &ctx) {
    const auto &opts = dynamic_cast<const ModelPredictiveControllerOptions &>(*ModelPredictiveController::opts);
    DownloadDecision decision{.SegmentID = ctx.NextSegmentID};

    const auto &bitRatesInKbps = ctx.VideoModel.get().BitRatesInKbps;
    const auto minBitRateInKbps = bitRatesInKbps.front(), maxBitRateInKbps = bitRatesInKbps.back();
    const auto maxBufferLevelInMs =
            static_cast<double>(ctx.MaxBufferSegmentCount) * ctx.VideoModel.get().SegmentDurationInMs;
    const auto targetBufferLevelInMs = maxBufferLevelInMs * opts.TargetBufferRatio;
    const auto throughputEstimateInKbps = ctx.ThroughputEstimator.get().EstimateInKbps();
    const auto lastBitRateInKbps = ctx.BufferedBitRatesInKbps[ctx.NextSegmentID - 1].value();

    static constexpr auto obj =
            "-Sum[-Sqrt[rMin * rMax] * omega * Subscript[u, k]^2, {k, K}]"
            "+ beta * Sum[((Subscript[x, k] - xOpt) / (xMax / 2))^2, {k, K}]"
            "+ gamma * rMin * rMax * (u0 - Subscript[u, 1])^2"
            "+ gamma * Sum[rMin * rMax * (Subscript[u, k - 1] - Subscript[u, k])^2, {k, 2, K}]";
    static constexpr auto cons =
            "Join[{Subscript[x, 1] == x0 - deltaT + omega * deltaT * Subscript[u, 1]}, "
            "Table[Subscript[x, k] == Subscript[x, k - 1] - deltaT + omega * deltaT * Subscript[u, k], {k, 2, K}], "
            "Table[Subscript[u, k] > 0, {k, K}], "
            "Table[Subscript[x, k] >= 0 && Subscript[x, k] <= xMax, {k, K}]]";
    static constexpr auto vars = "Join[Table[Subscript[u, k], {k, K}], Table[Subscript[x, k], {k, K}]]";
    static const auto body = fmt::format("QuadraticOptimization[{obj}, {cons}, {vars}, \"PrimalMinimizer\"]",
                                         "obj"_a = obj, "cons"_a = cons, "vars"_a = vars);

    std::vector<double> solutions;
    LLU::EvaluateFunction(stream,
                          {"rMin", "rMax", "xMax", "xOpt", "K", "deltaT", "beta", "gamma", "omega", "u0", "x0"}, body,
                          minBitRateInKbps, maxBitRateInKbps, maxBufferLevelInMs, targetBufferLevelInMs,
                          opts.WindowSize, opts.TimeIntervalInMs,
                          opts.BufferDeviationPenaltyFactor, opts.SwitchingCostFactor,
                          throughputEstimateInKbps, 1 / lastBitRateInKbps, ctx.BufferLevelInMs());
    stream.get() >> solutions;
    const auto nextBitRateInKbps =
            solutions.front() > 0 ? std::clamp(1 / solutions.front(), minBitRateInKbps, maxBitRateInKbps)
                                  : maxBitRateInKbps;
    const auto it = std::upper_bound(bitRatesInKbps.cbegin(), bitRatesInKbps.cend(), nextBitRateInKbps);
    decision.BitRateID = it != bitRatesInKbps.cbegin() ? it - bitRatesInKbps.cbegin() - 1 : 0;

    return decision;
}
