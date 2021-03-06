(* ::Package:: *)

Get["LibraryLinkUtilities`"];

BeginPackage["ABRSimulation`"];

Once@AppendTo[$Path, DirectoryName[$InputFileName]];

Get["VideoModel`"];
Get["NetworkModel`"];

ABRSessionSimulate::usage = "ABRSessionSimulate[videoModel, netModel, opts] simulates an ABR session for a video under \
the specified network model.\nABRSessionSimulate[videoModel, {netModel1, netModel2, ...}, opts] simulates multiple ABR \
sessions for a video under the specified network models.";

ABRSessionPlot::usage = "ABRSessionPlot[simData] visualizes the download activities during an ABR session.";

Begin["`Private`"];

LoadPacletLibrary[] := (
    Global`LLU`InitializePacletLibrary["ABRSimulationLink"];
    Global`LLU`WSTPFunctionSet[$ABRSessionSimulate];
);

UnloadPacletLibrary[] := Do[LibraryUnload["ABRSimulationLink"], 2];

LoadPacletLibrary[];

Options[ABRSessionSimulate] = {
    "Controller" -> {"ModelPredictiveController", Automatic},
    "ThroughputEstimator" -> {"ExponentialMovingAverageEstimator", Automatic},
    "SessionOptions" -> Automatic
};

ABRSessionSimulate[VideoModel[video_?AssociationQ], NetworkModel[net_?AssociationQ], OptionsPattern[]] := Module[
    {controllerType, controllerOpts, throughputEstimatorType, throughputEstimatorOpts, sessionOpts, simData, totalTime},

    controllerType = First@OptionValue["Controller"];
    If[(controllerOpts = Rest@OptionValue["Controller"]) == {Automatic},
        controllerOpts = Switch[controllerType,
            "ModelPredictiveController", {"TimeIntervalInMs" -> QuantityMagnitude[video["SegmentDuration"], "Milliseconds"]},
            _, {}
        ]
    ];
    throughputEstimatorType = First@OptionValue["ThroughputEstimator"];
    If[(throughputEstimatorOpts = Rest@OptionValue["ThroughputEstimator"]) == {Automatic}, throughputEstimatorOpts = {}];
    If[(sessionOpts = OptionValue["SessionOptions"]) == Automatic, sessionOpts = {}];

    simData = $ABRSessionSimulate[
        <|
            "SegmentDurationInMs" -> QuantityMagnitude[video["SegmentDuration"], "Milliseconds"],
            "EncodingBitRatesInKbps" -> QuantityMagnitude[video["EncodingBitRates"], "Kilobits" / "Seconds"],
            "SegmentByteCounts" -> QuantityMagnitude[video["SegmentSizes"], "Bytes"]
        |>,
        <|
            "DurationsInMs" -> QuantityMagnitude[net["Durations"], "Milliseconds"],
            "ThroughputsInKbps" -> QuantityMagnitude[net["Throughputs"], "Kilobits" / "Seconds"]
        |>,
        controllerType, controllerOpts,
        throughputEstimatorType, throughputEstimatorOpts,
        sessionOpts
    ];
    totalTime = Quantity[simData["TotalTimeInMs"], "Milliseconds"];

    Return@Dataset[
        <|
            "TotalTime" -> totalTime,
            "MaxBufferLevel" -> Quantity[simData["MaxBufferLevelInMs"], "Milliseconds"],
            "EncodingBitRates" -> video["EncodingBitRates"],
            "BufferedBitRates" -> QuantityArray[simData["BufferedBitRatesInKbps"], "Kilobits" / "Seconds"],
            "NetworkTimeSeries" -> TimeSeries[NetworkModel[net], totalTime],
            "DownloadTimeSeries" -> TimeSeries[
                Prepend[QuantityArray[simData["DownloadBitRatesInKbps"], "Kilobits" / "Seconds"], Quantity[0, "Kilobits" / "Seconds"]],
                {Prepend[Accumulate@QuantityMagnitude[QuantityArray[simData["DownloadDurationsInMs"], "Milliseconds"], "Seconds"], 0]},
                ResamplingMethod -> {"Interpolation", "HoldFrom" -> Right}
            ],
            "BufferTimeSeries" -> TimeSeries[
                QuantityArray[simData["BufferLevelsInMs"], "Milliseconds"],
                {QuantityMagnitude[QuantityArray[simData["BufferTimesInMs"], "Milliseconds"], "Seconds"]}
            ],
            "RebufferingPeriods" -> QuantityArray[simData["RebufferingPeriodsInMs"], "Milliseconds"],
            "FullBufferDelays" -> QuantityArray[simData["FullBufferDelaysInMs"], "Milliseconds"]
        |>,
        ItemDisplayFunction -> {
            "TotalTime" -> (QuantityForm[NumberForm[UnitConvert[#, "Minutes"], 3], "Abbreviation"] &),
            "MaxBufferLevel" -> (QuantityForm[NumberForm[UnitConvert[#, "Seconds"], 3], "Abbreviation"] &),
            "EncodingBitRates" -> (QuantityForm[NumberForm[Normal@UnitConvert[#, "Megabits" / "Seconds"], 3], "Abbreviation"] &),
            "BufferedBitRates" -> (Short@QuantityForm[NumberForm[Normal@UnitConvert[#, "Megabits" / "Seconds"], 3], "Abbreviation"] &),
            ("RebufferingPeriods" | "FullBufferDelays") ->
                    (Short@QuantityForm[NumberForm[Normal@UnitConvert[#, "Seconds"], 3], "Abbreviation"] &)
        }
    ]
];

ABRSessionSimulate[videoModel_VideoModel, netModels_ /; VectorQ, opts : OptionsPattern[]] :=
        Dataset@Normal@ParallelTable[ABRSessionSimulate[videoModel, netModel, opts], {netModel, netModels}];

ABRSessionPlot[simData_Dataset] := Module[
    {totalSeconds, maxBufferSeconds, downloadPlot, rebufferingLines, bufferPlot},

    totalSeconds = QuantityMagnitude[simData["TotalTime"], "Seconds"];
    maxBufferSeconds = QuantityMagnitude[simData["MaxBufferLevel"], "Seconds"];

    downloadPlot = ListStepPlot[
        {simData["DownloadTimeSeries"], simData["NetworkTimeSeries"]}, Left,
        GridLines -> {None, Normal@simData["EncodingBitRates"]}, GridLinesStyle -> Directive[Gray, Dashing[0, Tiny]],
        TargetUnits -> "Megabits" / "Seconds", AxesOrigin -> {0, 0},
        PlotRange -> {{0, totalSeconds}, Automatic}, PlotRangePadding -> {0.4, Scaled[0.02]},
        Frame -> True, FrameLabel -> {"Time (s)", "Bit Rate (Mb/s)"},
        PlotLegends -> Placed[{"Download Bit Rate", "Network Throughput"}, Below],
        AspectRatio -> Full, ImageSize -> {Full, Small}
    ];

    rebufferingLines = If[Length@simData["RebufferingPeriods"] == 0, {},
        Style[Line /@ Map[{#, 0} &, QuantityMagnitude[simData["RebufferingPeriods"], "Seconds"], {2}], Red]];
    bufferPlot = ListLinePlot[
        simData["BufferTimeSeries"], Epilog -> rebufferingLines,
        GridLines -> {None, {simData["MaxBufferLevel"]}}, GridLinesStyle -> Directive[Gray, Dashing[0, Tiny]],
        TargetUnits -> "Seconds", AxesOrigin -> {0, 0},
        PlotRange -> {{0, totalSeconds}, {0, maxBufferSeconds}}, PlotRangePadding -> {0.4, Scaled[0.02]},
        Frame -> True, FrameLabel -> {"Time (s)", "Buffer Level (s)"},
        PlotLegends -> Placed[{"Current Buffer Level"}, Below],
        AspectRatio -> Full, ImageSize -> {Full, Small}
    ];

    Manipulate[
        Column[{
            Show[downloadPlot, PlotRange -> {{firstSecond, firstSecond + plotSeconds}, Automatic}],
            Show[bufferPlot, PlotRange -> {{firstSecond, firstSecond + plotSeconds}, {0, maxBufferSeconds}}]
        }, ItemSize -> Full],
        {{firstSecond, 0, "First Second"}, 0, totalSeconds},
        {{plotSeconds, 60, "Plot Seconds"}, 10, 120, 10}
    ]
];

End[];

EndPackage[];
