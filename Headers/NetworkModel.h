#pragma once

#include "Core.h"

struct DownloadData {
    int TimeInMs;
    int DownloadedByteCount;
};

class NetworkModel {
public:
    template<std::input_iterator InputIt>
    NetworkModel(InputIt durationsInMsFirst, InputIt durationsInMsLast,
                 InputIt bandwidthsInKBpsFirst, InputIt bandwidthsInKBpsLast);

    DownloadData Download(int byteCount);

    void Delay(int timeInMs);

private:
    int period = 0, timeInPeriodInMs = 0;
    std::vector<int> durationsInMs, bandwidthsInKBps;
};

template<std::input_iterator InputIt>
NetworkModel::NetworkModel(InputIt durationsInMsFirst, InputIt durationsInMsLast,
                           InputIt bandwidthsInKBpsFirst, InputIt bandwidthsInKBpsLast) {
    if (std::distance(durationsInMsFirst, durationsInMsLast) !=
        std::distance(bandwidthsInKBpsFirst, bandwidthsInKBpsLast))
        throw std::runtime_error("The duration list and bandwidth list have to be of equal size.");

    durationsInMs.assign(durationsInMsFirst, durationsInMsLast);
    bandwidthsInKBps.assign(bandwidthsInKBpsFirst, bandwidthsInKBpsLast);
}
