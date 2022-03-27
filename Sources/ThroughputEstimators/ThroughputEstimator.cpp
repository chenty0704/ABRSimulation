#include "ThroughputEstimators/ThroughputEstimator.h"

void ThroughputEstimator::Push(DownloadData data) {
    const auto throughputInKbps = data.ByteCount * CHAR_BIT / data.TimeInMs;
    timeInMs += data.TimeInMs;
    durationsInMs.push_back(data.TimeInMs);
    throughputsInKbps.push_back(throughputInKbps);
}
