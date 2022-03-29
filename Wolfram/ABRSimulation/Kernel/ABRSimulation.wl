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
  "ControllerType" -> "ThroughputBasedController",
  "ControllerOptions" -> Automatic,
  "ThroughputEstimatorType" -> "ExponentialMovingAverageEstimator",
  "ThroughputEstimatorOptions" -> Automatic,
  "SessionOptions" -> Automatic
};
ABRSessionSimulate[videoModelFile_String, networkModelFile_String, OptionsPattern[]] := With[
  {simData = $ABRSessionSimulate[
    videoModelFile, networkModelFile,
    OptionValue["ControllerType"],
    If[OptionValue["ControllerOptions"] == Automatic, <||>, OptionValue["ControllerOptions"]],
    OptionValue["ThroughputEstimatorType"],
    If[OptionValue["ThroughputEstimatorOptions"] == Automatic, <||>, OptionValue["ThroughputEstimatorOptions"]],
    If[OptionValue["SessionOptions"] == Automatic, <||>, OptionValue["SessionOptions"]]
  ]},
  Return[<|
    "TotalTime" -> UnitConvert[Quantity[simData["TotalTimeInMs"], "Milliseconds"], "Seconds"],
    "BufferedBitRates" -> QuantityArray[simData["BufferedBitRatesInKbps"], "Kilobits" / "Seconds"],
    "RebufferingDurations" -> QuantityArray[simData["RebufferingDurationsInMs"], "Milliseconds"],
    "FullBufferDelays" -> QuantityArray[simData["FullBufferDelaysInMs"], "Milliseconds"],
    "DownloadDurations" -> QuantityArray[simData["DownloadDurationsInMs"], "Milliseconds"],
    "DownloadBitRates" -> QuantityArray[simData["DownloadBitRatesInKbps"], "Kilobits" / "Seconds"]
  |>]
];

End[];

EndPackage[];
