#include "ABRControllers/ModelPredictiveController.h"
#include "Interface/CoreInterface.h"

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

    const auto objective = fmt::format(
            "-Sum[-Sqrt[{rMin} * {rMax}] * {omega} * Subscript[u, k]^2, {{k, {K}}}]"
            "+ {beta} * Sum[((Subscript[x, k] - {xOpt}) / ({xMax} / 2))^2, {{k, {K}}}]"
            "+ {gamma} * {rMin} * {rMax} * ({u0}- Subscript[u, 1])^2"
            "+ {gamma} * Sum[{rMin} * {rMax} * (Subscript[u, k - 1] - Subscript[u, k])^2, {{k, 2, {K}}}]",
            "rMin"_a = minBitRateInKbps, "rMax"_a = maxBitRateInKbps,
            "xMax"_a = maxBufferLevelInMs, "xOpt"_a = targetBufferLevelInMs,
            "beta"_a = opts.BufferDeviationPenaltyFactor, "gamma"_a = opts.SwitchingCostFactor,
            "K"_a = opts.WindowSize, "omega"_a = throughputEstimateInKbps, "u0"_a = 1 / lastBitRateInKbps
    );
    const auto constraints = fmt::format(
            "Join[{{Subscript[x, 1] == {x0} - {deltaT} + {omega} * {deltaT} * Subscript[u, 1]}}, "
            "Table[Subscript[x, k] == Subscript[x, k - 1] - {deltaT} + {omega} * {deltaT} * Subscript[u, k], {{k, 2, {K}}}], "
            "Table[Subscript[u, k] > 0, {{k, {K}}}], "
            "Table[Subscript[x, k] >= 0 && Subscript[x, k] <= {xMax}, {{k, {K}}}]]",
            "xMax"_a = maxBufferLevelInMs, "deltaT"_a = opts.TimeIntervalInMs,
            "K"_a = opts.WindowSize, "omega"_a = throughputEstimateInKbps, "x0"_a = ctx.BufferLevelInMs()
    );
    const auto variables = fmt::format(
            "Join[Table[Subscript[u, k], {{k, {K}}}], Table[Subscript[x, k], {{k, {K}}}]]",
            "K"_a = opts.WindowSize
    );

    std::vector<double> solutions;
    LLU::Evaluate(stream.get(), fmt::format(
            "QuadraticOptimization[{obj}, {cons}, {vars}, \"PrimalMinimizer\"]",
            "obj"_a = objective, "cons"_a = constraints, "vars"_a = variables
    ));
    stream.get() >> LLU::WS::Function("ReturnPacket", 1) >> solutions;
    const auto nextBitRateInKbps = solutions.front() > 0 ?
                                   std::clamp(solutions.front(), minBitRateInKbps, maxBitRateInKbps) : maxBitRateInKbps;
    const auto it = std::upper_bound(bitRatesInKbps.cbegin(), bitRatesInKbps.cend(), nextBitRateInKbps);
    decision.BitRateID = it != bitRatesInKbps.cbegin() ? it - bitRatesInKbps.cbegin() - 1 : 0;

    return decision;
}
