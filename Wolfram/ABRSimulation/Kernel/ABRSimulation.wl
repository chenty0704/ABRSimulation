(* ::Package:: *)

BeginPackage["ABRSimulation`"];

ABRSessionSimulate::usage = "ABRSessionSimulate[videoModelFile, networkModelFile, options] simulates an ABR session \
using the specified options.";

Begin["`Private`"];

$PacletDirectory = FileNameDrop[$InputFileName, -2];
AppendTo[$LibraryPath, FileNameJoin[{$PacletDirectory, "LibraryResources", $SystemID}]];
Get[FileNameJoin[{$PacletDirectory, "LibraryResources", "LibraryLinkUtilities.wl"}]];
`LLU`InitializePacletLibrary["ABRSimulationInterface"];
`LLU`LazyWSTPFunctionSet @@@ {
  {$ABRSessionSimulate, "ABRSessionSimulate"}
};

Options[ABRSessionSimulate] = {
  "Controller" -> {"ThroughputBasedController", Automatic},
  "ThroughputEstimator" -> {"ExponentialMovingAverageEstimator", Automatic},
  "SessionOptions" -> Automatic
};
ABRSessionSimulate[videoModelFile_String, networkModelFile_String, OptionsPattern[]] := Module[
  {controllerType, controllerOpts, throughputEstimatorType, throughputEstimatorOpts, sessionOpts, simData},

  controllerType = First@OptionValue["Controller"];
  controllerOpts = Rest@OptionValue["Controller"];
  If[controllerOpts == {Automatic}, controllerOpts = {}];
  throughputEstimatorType = First@OptionValue["ThroughputEstimator"];
  throughputEstimatorOpts = Rest@OptionValue["ThroughputEstimator"];
  If[throughputEstimatorOpts == {Automatic}, throughputEstimatorOpts = {}];
  sessionOpts = OptionValue["SessionOptions"];
  If[sessionOpts == Automatic, sessionOpts = {}];

  simData = $ABRSessionSimulate[
    videoModelFile, networkModelFile,
    controllerType, controllerOpts,
    throughputEstimatorType, throughputEstimatorOpts,
    sessionOpts
  ];
  Return[<|
    "TotalTime" -> UnitConvert[Quantity[N@simData["TotalTimeInMs"], "Milliseconds"], "Seconds"],
    "BufferedBitRates" -> QuantityArray[N@simData["BufferedBitRatesInKbps"], "Kilobits" / "Seconds"],
    "RebufferingDurations" -> QuantityArray[N@simData["RebufferingDurationsInMs"], "Milliseconds"],
    "FullBufferDelays" -> QuantityArray[N@simData["FullBufferDelaysInMs"], "Milliseconds"],
    "DownloadTimeSeries" -> TimeSeries[QuantityArray[N@Prepend[simData["DownloadBitRatesInKbps"], 0], "Kilobits" / "Seconds"],
      {Prepend[QuantityMagnitude[Accumulate[QuantityArray[N@simData["DownloadDurationsInMs"], "Milliseconds"]], "Seconds"], 0]},
      ResamplingMethod -> {"Interpolation", "HoldFrom" -> Right}]
  |>]
];

End[];

EndPackage[];
