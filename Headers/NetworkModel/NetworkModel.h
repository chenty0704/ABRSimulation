#pragma once

#include "Core/Core.h"

struct DownloadData {
    size_t TimeInMs;
    size_t ByteCount;
};

class NetworkModel {
public:
    static NetworkModel Import(std::string_view fileName);

    DownloadData Download(size_t byteCount);

    void Delay(size_t timeInMs);

private:
    size_t period = 0, timeInPeriodInMs = 0;
    std::vector<size_t> durationsInMs, throughputsInKbps;
};
