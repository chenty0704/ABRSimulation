VideoModel::usage = "VideoModel[assoc] represents a video model.";

VideoModelRepeat::usage = "VideoModelRepeat[videoModel, duration] repeats a video model for at least the specified duration.";

Begin["`Private`"];

ImportExport`RegisterImport["VideoModel", VideoModelImport];

VideoModelImport[fileName_String] := With[
    {rawData = Import[fileName, "RawJson"]},

    {
        "Data" -> VideoModel@<|
            "SegmentDuration" -> Quantity[N@rawData["SegmentDurationInMs"], "Milliseconds"],
            "EncodingBitRates" -> QuantityArray[N@rawData["EncodingBitRatesInKbps"], "Kilobits" / "Seconds"],
            "SegmentSizes" -> QuantityArray[rawData["SegmentByteCounts"], "Bytes"]
        |>
    }
];

VideoModel /: Information[VideoModel[video_?AssociationQ]] := With[
    {bitRates = video["SegmentSizes"] / video["SegmentDuration"]},

    InformationData@<|
        "ObjectType" -> "VideoModel",
        "TotalDuration" -> NumberForm[UnitConvert[video["SegmentDuration"] * Length@video["SegmentSizes"],
            MixedUnit[{"Minutes", "Seconds"}]], 3],
        "SegmentDuration" -> UnitConvert[video["SegmentDuration"], "Seconds"],
        "BitRates" -> Dataset[
            AssociationThread[Range@Length@video["EncodingBitRates"] ->
                    Table[<|
                        "EncodingBitRate" -> UnitConvert[video["EncodingBitRates"][[i]], "Megabits" / "Seconds"],
                        "Mean" -> UnitConvert[Mean@bitRates[[All, i]], "Megabits" / "Seconds"],
                        "StandardDeviation" -> UnitConvert[StandardDeviation@bitRates[[All, i]], "Megabits" / "Seconds"]
                    |>, {i, Length@video["EncodingBitRates"]}]],
            HeaderStyle -> "DialogStyle", ItemStyle -> "DialogStyle",
            ItemDisplayFunction -> (QuantityForm[NumberForm[#, 3], "Abbreviation"] &)
        ]
    |>
];

VideoModel /: VideoModelRepeat[VideoModel[video_?AssociationQ], duration_?QuantityQ] := With[
    {segmentCount = Ceiling[duration / video["SegmentDuration"]]},

    VideoModel@<|
        "SegmentDuration" -> video["SegmentDuration"],
        "EncodingBitRates" -> video["EncodingBitRates"],
        "SegmentSizes" -> Take[
            Join @@ Table[video["SegmentSizes"], Ceiling[segmentCount / Length@video["SegmentSizes"]]],
            segmentCount
        ]
    |>
];

End[];
