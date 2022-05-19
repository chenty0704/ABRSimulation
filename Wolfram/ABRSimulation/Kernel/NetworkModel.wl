NetworkModel::usage = "NetworkModel[assoc] represents a network model.";

Begin["`Private`"];

ImportExport`RegisterImport["NetworkModel", NetworkModelImport];

NetworkModelImport[fileName_String] := With[
    {rawData = Import[fileName, "RawJson"]},

    {
        "Data" -> NetworkModel@<|
            "Durations" -> QuantityArray[N@rawData["DurationsInMs"], "Milliseconds"],
            "Throughputs" -> QuantityArray[N@rawData["ThroughputsInKbps"], "Kilobits" / "Seconds"]
        |>
    }
];

NetworkModel /: Information[NetworkModel[net_?AssociationQ]] := InformationData@<|
    "ObjectType" -> "NetworkModel",
    "TotalDuration" -> NumberForm[UnitConvert[Total@net["Durations"], MixedUnit[{"Minutes", "Seconds"}]], 3],
    "PeriodCount" -> Length@net["Durations"],
    "MeanThroughput" -> NumberForm[UnitConvert[Mean@net["Throughputs"], "Megabits" / "Seconds"], 3],
    "StandardDeviation" -> NumberForm[UnitConvert[StandardDeviation@net["Throughputs"], "Megabits" / "Seconds"], 3]
|>;

NetworkModel /: TimeSeries[NetworkModel[net_?AssociationQ], duration_?QuantityQ] := With[
    {copyCount = Ceiling[duration / Total@net["Durations"]]},

    TimeSeriesWindow[TimeSeries[
        Prepend[Join @@ Table[net["Throughputs"], copyCount], Quantity[0, "Kilobits" / "Seconds"]],
        {Prepend[Accumulate@QuantityMagnitude[Join @@ Table[net["Durations"], copyCount], "Seconds"], 0]},
        ResamplingMethod -> {"Interpolation", "HoldFrom" -> Right}
    ], {0, QuantityMagnitude[duration, "Seconds"]}]
];

End[];
