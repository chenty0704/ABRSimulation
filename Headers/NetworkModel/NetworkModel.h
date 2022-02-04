#pragma once

#include "Core/Core.h"

struct DownloadData {
    int TimeInMs;
    int ByteCount;
};

class NetworkModel {
public:
    static NetworkModel Import(std::string_view fileName);

    DownloadData Download(int byteCount);

    void Delay(int timeInMs);

private:
    int period = 0, timeInPeriodInMs = 0;
    std::vector<int> durationsInMs, throughputsInKbps;
};
