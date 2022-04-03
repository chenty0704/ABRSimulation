#pragma once

#include "Core/Core.h"

struct DownloadData {
    double TimeInMs;
    size_t ByteCount;
};

class NetworkModel {
public:
    static NetworkModel Import(std::string_view fileName);

    DownloadData Download(size_t byteCount);

    void Delay(double timeInMs);

private:
    size_t period = 0;
    double timeInPeriodInMs = 0;
    std::vector<double> durationsInMs, throughputsInKbps;
};
