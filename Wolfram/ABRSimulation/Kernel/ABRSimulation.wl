(* ::Package:: *)

BeginPackage["ABRSimulation`"];

ImportVideoModel::usage = "ImportVideoModel[videoModelFile] imports a video model from a file.";

ImportNetworkModel::usage = "ImportNetworkModel[networkModelFile] imports a network model from a file";

ABRSessionSimulate::usage = "ABRSessionSimulate[videoModelFile, networkModelFile, opts] simulates an ABR session \
using the specified options.";

ABRSessionPlot::usage = "ABRSessionPlot[simData] plots the download activities of an ABR session.";

Begin["`Private`"];

$PacletDirectory = FileNameDrop[$InputFileName, -2];
AppendTo[$LibraryPath, FileNameJoin[{$PacletDirectory, "LibraryResources", $SystemID}]];
Get[FileNameJoin[{$PacletDirectory, "LibraryResources", "LibraryLinkUtilities.wl"}]];
`LLU`InitializePacletLibrary["ABRSimulationInterface"];
`LLU`LazyWSTPFunctionSet @@@ {
  {$ABRSessionSimulate, "ABRSessionSimulate"}
};

FromDurationsAndValues[durations_QuantityArray, values_QuantityArray] := TimeSeries[
  Prepend[values, Quantity[0, QuantityUnit@First[values]]],
  {Prepend[Accumulate[QuantityMagnitude[durations, "Seconds"]], 0]},
  ResamplingMethod -> {"Interpolation", "HoldFrom" -> Right}
];

ImportVideoModel[videoModelFile_String] := With[
  {videoModel = Import[videoModelFile, "RawJSON"]},
  Return[<|
    "SegmentDuration" -> Quantity[videoModel["SegmentDurationInMs"], "Milliseconds"],
    "BitRates" -> QuantityArray[videoModel["BitRatesInKbps"], "Kilobits" / "Seconds"],
    "SegmentSizes" -> QuantityArray[videoModel["SegmentByteCounts"], "Bytes"]
  |>]
];

ImportNetworkModel[networkModelFile_String] := With[
  {networkModel = Import[networkModelFile, "RawJSON"]},
  Return[<|
    "Durations" -> QuantityArray[networkModel["DurationsInMs"], "Milliseconds"],
    "Throughputs" -> QuantityArray[networkModel["ThroughputsInKbps"], "Kilobits" / "Seconds"]
  |>]
];

NetworkTimeSeries[networkModel_Association, length_Quantity] := With[
  {copyCount = Ceiling[length / Total[networkModel["Durations"]]]},
  Return[FromDurationsAndValues[
    Join @@ Table[networkModel["Durations"], copyCount],
    Join @@ Table[networkModel["Throughputs"], copyCount]
  ]]
];

Options[ABRSessionSimulate] = {
  "Controller" -> {"ThroughputBasedController", Automatic},
  "ThroughputEstimator" -> {"ExponentialMovingAverageEstimator", Automatic},
  "MaxBufferSegmentCount" -> 5
};
ABRSessionSimulate[videoModelFile_String, networkModelFile_String, OptionsPattern[]] := Module[
  {videoModel, controllerType, controllerOpts, throughputEstimatorType, throughputEstimatorOpts, sessionOpts, simData, totalTime},

  videoModel = ImportVideoModel[videoModelFile];
  controllerType = First@OptionValue["Controller"];
  controllerOpts = Rest@OptionValue["Controller"];
  If[controllerOpts == {Automatic}, controllerOpts = {}];
  throughputEstimatorType = First@OptionValue["ThroughputEstimator"];
  throughputEstimatorOpts = Rest@OptionValue["ThroughputEstimator"];
  If[throughputEstimatorOpts == {Automatic}, throughputEstimatorOpts = {}];
  sessionOpts = {
    "MaxBufferSegmentCount" -> OptionValue["MaxBufferSegmentCount"]
  };

  simData = $ABRSessionSimulate[
    videoModelFile, networkModelFile,
    controllerType, controllerOpts,
    throughputEstimatorType, throughputEstimatorOpts,
    sessionOpts
  ];
  totalTime = Quantity[simData["TotalTimeInMs"], "Milliseconds"];
  Return[<|
    "TotalTime" -> totalTime,
    "MaxBufferLevel" -> OptionValue["MaxBufferSegmentCount"] * videoModel["SegmentDuration"],
    "EncodingBitRates" -> videoModel["BitRates"],
    "BufferedBitRates" -> QuantityArray[simData["BufferedBitRatesInKbps"], "Kilobits" / "Seconds"],
    "NetworkTimeSeries" -> NetworkTimeSeries[ImportNetworkModel[networkModelFile], totalTime],
    "DownloadTimeSeries" -> FromDurationsAndValues[
      QuantityArray[simData["DownloadDurationsInMs"], "Milliseconds"],
      QuantityArray[simData["DownloadBitRatesInKbps"], "Kilobits" / "Seconds"]
    ],
    "BufferTimeSeries" -> TimeSeries[
      QuantityArray[simData["BufferLevelsInMs"], "Milliseconds"],
      {QuantityMagnitude[QuantityArray[simData["BufferTimesInMs"], "Milliseconds"], "Seconds"]}
    ],
    "RebufferingDurations" -> QuantityArray[simData["RebufferingDurationsInMs"], "Milliseconds"],
    "FullBufferDelays" -> QuantityArray[simData["FullBufferDelaysInMs"], "Milliseconds"]
  |>]
];

ABRSessionPlot[simData_Association] := Module[
  {totalSeconds, downloadPlot, bitRateRefLines, bufferPlot, maxBufferRefLine},

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
    PlotRange -> All, AxesLabel -> {"Time (s)", "Bit Rate (Mb/s)"},
    AspectRatio -> Full, ImageSize -> {Full, Tiny}
  ];
  bufferPlot = ListLinePlot[
    simData["BufferTimeSeries"],
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
  Return[Column[{downloadPlot, bufferPlot}, ItemSize -> Full]]
];

End[];

EndPackage[];
