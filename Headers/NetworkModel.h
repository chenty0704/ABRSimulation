#pragma once

#include "Core.h"

struct DownloadData {
    int TimeInMs;
    int DownloadedBitCount;
};

class NetworkModel {
public:
    template<std::input_iterator InputIt>
    NetworkModel(InputIt durationsInMsFirst, InputIt durationsInMsLast,
                 InputIt throughputsInKbpsFirst, InputIt throughputsInKbpsLast);

    DownloadData Download(int bitCount);

    void Delay(int timeInMs);

private:
    int period = 0, timeInPeriodInMs = 0;
    std::vector<int> durationsInMs, throughputsInKbps;
};

template<std::input_iterator InputIt>
NetworkModel::NetworkModel(InputIt durationsInMsFirst, InputIt durationsInMsLast,
                           InputIt throughputsInKbpsFirst, InputIt throughputsInKbpsLast) {
    if (std::distance(durationsInMsFirst, durationsInMsLast) !=
        std::distance(throughputsInKbpsFirst, throughputsInKbpsLast))
        throw std::runtime_error("The duration and throughput list have to be of equal sizes.");

    durationsInMs.assign(durationsInMsFirst, durationsInMsLast);
    throughputsInKbps.assign(throughputsInKbpsFirst, throughputsInKbpsLast);
}
