#pragma once

#include "Core/Core.h"

struct DownloadData {
    double TimeInMs;
    size_t ByteCount;
};

class NetworkModel {
public:
    std::vector<double> DurationsInMs;
    std::vector<double> ThroughputsInKbps;

    DownloadData Download(size_t byteCount);

    void Delay(double timeInMs);

private:
    size_t period = 0;
    double timeInPeriodInMs = 0;
};
