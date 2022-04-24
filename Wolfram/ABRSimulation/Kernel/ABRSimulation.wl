(* ::Package:: *)

BeginPackage["ABRSimulation`"];

ImportVideoModel::usage = "ImportVideoModel[videoModelFile] imports a video model from a file.";

VideoModelData::usage = "VideoModelData[videoModel, opts] gives properties of a video model.";

ImportNetworkModel::usage = "ImportNetworkModel[networkModelFile] imports a network model from a file";

ABRSessionSimulate::usage = "ABRSessionSimulate[videoModel, networkModel, opts] simulates an ABR session for a video \
under the specified network model.\nABRSessionSimulate[videoModel, {networkModel1, networkModel2, ...}, opts] simulates \
multiple ABR session for a video under the specified network models.";

ABRSessionPlot::usage = "ABRSessionPlot[simData] visualizes the download activities of an ABR session.";

Begin["`Private`"];

$PacletDirectory = FileNameDrop[$InputFileName, -2];
AppendTo[$Path, FileNameJoin[{$PacletDirectory, "Kernel"}]];
AppendTo[$Path, FileNameJoin[{$PacletDirectory, "LibraryResources"}]];
AppendTo[$LibraryPath, FileNameJoin[{$PacletDirectory, "LibraryResources", $SystemID}]];

Get["LibraryLinkUtilities`"];

`LLU`InitializePacletLibrary["ABRSimulationInterface"];
`LLU`WSTPFunctionSet @@@ {
  {$ABRSessionSimulate, "ABRSessionSimulate"}
};

FromDurationsAndValues[durations_QuantityArray, values_QuantityArray] := TimeSeries[
  Prepend[values, Quantity[0, QuantityUnit@First[values]]],
  {Prepend[Accumulate[QuantityMagnitude[durations, "Seconds"]], 0]},
  ResamplingMethod -> {"Interpolation", "HoldFrom" -> Right}
];

ImportVideoModel[videoModelFile_String] := With[
  {videoModel = Import[videoModelFile, "RawJSON"]},
  Return@<|
    "SegmentDuration" -> Quantity[N@videoModel["SegmentDurationInMs"], "Milliseconds"],
    "EncodingBitRates" -> QuantityArray[N@videoModel["EncodingBitRatesInKbps"], "Kilobits" / "Seconds"],
    "SegmentSizes" -> QuantityArray[videoModel["SegmentByteCounts"], "Bytes"]
  |>
];

Options[VideoModelData] = {
  TargetUnits -> "Kilobits" / "Seconds"
};

VideoModelData[videoModel_Association, OptionsPattern[]] := With[
  {bitRates = videoModel["SegmentSizes"] / videoModel["SegmentDuration"]},
  Return@Dataset@Table[<|
    "EncodingBitRate" -> videoModel["EncodingBitRates"][[i]],
    "Mean" -> UnitConvert[Mean@bitRates[[All, i]], OptionValue[TargetUnits]],
    "StandardDeviation" -> UnitConvert[StandardDeviation@bitRates[[All, i]], OptionValue[TargetUnits]]
  |>, {i, Length@videoModel["EncodingBitRates"]}]
];

ImportNetworkModel[networkModelFile_String] := With[
  {networkModel = Import[networkModelFile, "RawJSON"]},
  Return@<|
    "Durations" -> QuantityArray[N@networkModel["DurationsInMs"], "Milliseconds"],
    "Throughputs" -> QuantityArray[N@networkModel["ThroughputsInKbps"], "Kilobits" / "Seconds"]
  |>
];

NetworkTimeSeries[networkModel_Association, length_Quantity] := With[
  {copyCount = Ceiling[length / Total@networkModel["Durations"]]},
  Return@FromDurationsAndValues[
    Join @@ Table[networkModel["Durations"], copyCount],
    Join @@ Table[networkModel["Throughputs"], copyCount]
  ]
];

Options[ABRSessionSimulate] = {
  "Controller" -> {"ModelPredictiveController", Automatic},
  "ThroughputEstimator" -> {"ExponentialMovingAverageEstimator", Automatic},
  "SessionOptions" -> Automatic,
  "PlaybackTime" -> Automatic
};

ABRSessionSimulate[videoModel_Association, networkModel_Association, OptionsPattern[]] := Module[
  {controllerType, controllerOpts, throughputEstimatorType, throughputEstimatorOpts, sessionOpts, playbackTime, simData, totalTime},

  controllerType = First@OptionValue["Controller"];
  controllerOpts = Rest@OptionValue["Controller"];
  If[controllerOpts == {Automatic}, controllerOpts = Switch[controllerType,
    "ModelPredictiveController", {"TimeIntervalInMs" -> QuantityMagnitude[videoModel["SegmentDuration"], "Milliseconds"]},
    _, {}
  ]];
  throughputEstimatorType = First@OptionValue["ThroughputEstimator"];
  throughputEstimatorOpts = Rest@OptionValue["ThroughputEstimator"];
  If[throughputEstimatorOpts == {Automatic}, throughputEstimatorOpts = {}];
  sessionOpts = OptionValue["SessionOptions"];
  If[sessionOpts == Automatic, sessionOpts = {}];
  playbackTime = OptionValue["PlaybackTime"];

  simData = $ABRSessionSimulate[
    <|
      "SegmentDurationInMs" -> QuantityMagnitude[videoModel["SegmentDuration"], "Milliseconds"],
      "EncodingBitRatesInKbps" -> QuantityMagnitude[videoModel["EncodingBitRates"], "Kilobits" / "Seconds"],
      "SegmentByteCounts" -> QuantityMagnitude[
        If[playbackTime === Automatic,
          videoModel["SegmentSizes"],
          With[{segmentCount = Ceiling[playbackTime / videoModel["SegmentDuration"]]},
            Take[Join @@ Table[videoModel["SegmentSizes"], Ceiling[segmentCount / Length@videoModel["SegmentSizes"]]], segmentCount]
          ]
        ], "Bytes"]
    |>,
    <|
      "DurationsInMs" -> QuantityMagnitude[networkModel["Durations"], "Milliseconds"],
      "ThroughputsInKbps" -> QuantityMagnitude[networkModel["Throughputs"], "Kilobits" / "Seconds"]
    |>,
    controllerType, controllerOpts,
    throughputEstimatorType, throughputEstimatorOpts,
    sessionOpts
  ];
  totalTime = Quantity[simData["TotalTimeInMs"], "Milliseconds"];
  Return@Dataset@<|
    "TotalTime" -> totalTime,
    "MaxBufferLevel" -> Quantity[simData["MaxBufferLevelInMs"], "Milliseconds"],
    "EncodingBitRates" -> videoModel["EncodingBitRates"],
    "BufferedBitRates" -> QuantityArray[simData["BufferedBitRatesInKbps"], "Kilobits" / "Seconds"],
    "NetworkTimeSeries" -> NetworkTimeSeries[networkModel, totalTime],
    "DownloadTimeSeries" -> FromDurationsAndValues[
      QuantityArray[simData["DownloadDurationsInMs"], "Milliseconds"],
      QuantityArray[simData["DownloadBitRatesInKbps"], "Kilobits" / "Seconds"]
    ],
    "BufferTimeSeries" -> TimeSeries[
      QuantityArray[simData["BufferLevelsInMs"], "Milliseconds"],
      {QuantityMagnitude[QuantityArray[simData["BufferTimesInMs"], "Milliseconds"], "Seconds"]}
    ],
    "RebufferingPeriods" -> QuantityArray[simData["RebufferingPeriodsInMs"], "Milliseconds"],
    "FullBufferDelays" -> QuantityArray[simData["FullBufferDelaysInMs"], "Milliseconds"]
  |>
];

ABRSessionSimulate[videoModel_Association, networkModels_List, opts : OptionsPattern[]] :=
    Dataset@Normal@ParallelTable[ABRSessionSimulate[videoModel, networkModel, opts], {networkModel, networkModels}];

ABRSessionPlot[simData_Dataset] := Module[
  {totalSeconds, downloadPlot, bitRateRefLines, rebufferingLines, bufferPlot, maxBufferRefLine},

  totalSeconds = QuantityMagnitude[simData["TotalTime"], "Seconds"];
  downloadPlot = Plot[
    {simData["DownloadTimeSeries"]["PathFunction"][t], Style[simData["NetworkTimeSeries"]["PathFunction"][t]]},
    {t, 0, totalSeconds}, TargetUnits -> "Megabits" / "Seconds", PlotLegends -> {"Download Bit Rate", "Network Throughput"}
  ];
  bitRateRefLines = Plot[
    Evaluate@Style[Normal@simData["EncodingBitRates"], Gray, Dotted], {t, 0, totalSeconds},
    TargetUnits -> "Megabits" / "Seconds", PlotLegends -> {"Encoding Bit Rates"}
  ];
  downloadPlot = Show[
    downloadPlot, bitRateRefLines,
    AxesLabel -> {"Time (s)", "Bit Rate (Mb/s)"},
    AspectRatio -> Full, ImageSize -> {Full, Tiny}
  ];
  rebufferingLines = If[Length@simData["RebufferingPeriods"] == 0, {},
    Style[Line /@ Map[{#, 0} &, QuantityMagnitude[simData["RebufferingPeriods"], "Seconds"], {2}], Red]
  ];
  bufferPlot = ListLinePlot[
    simData["BufferTimeSeries"], Epilog -> rebufferingLines,
    TargetUnits -> "Seconds", PlotLegends -> {"Current Buffer Level"}
  ];
  maxBufferRefLine = Plot[
    Style[simData["MaxBufferLevel"], Gray, Dotted], {t, 0, totalSeconds},
    TargetUnits -> "Seconds", PlotLegends -> {"Max Buffer Level"}
  ];
  bufferPlot = Show[
    bufferPlot, maxBufferRefLine,
    PlotRange -> All, AxesLabel -> {"Time (s)", "Buffer Level (s)"},
    AspectRatio -> Full, ImageSize -> {Full, Tiny}
  ];
  Return@Column[{downloadPlot, bufferPlot}, ItemSize -> Full]
];

End[];

EndPackage[];
