#pragma once

#include "Core/Core.h"

struct DownloadData {
    int TimeInMs;
    int DownloadedBitCount;
};

class NetworkModel {
public:
    static NetworkModel Import(std::string_view fileName);

    DownloadData Download(int bitCount);

    void Delay(int timeInMs);

private:
    NetworkModel() = default;

    int period = 0, timeInPeriodInMs = 0;
    std::vector<int> durationsInMs, throughputsInKbps;
};
