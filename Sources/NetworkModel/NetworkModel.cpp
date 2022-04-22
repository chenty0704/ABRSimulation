#include "NetworkModel/NetworkModel.h"

DownloadData NetworkModel::Download(size_t byteCount) {
    size_t bitCount = 0;
    DownloadData data{0, 0};
    while (bitCount < byteCount * CHAR_BIT) {
        const auto restTimeInPeriodInMs = DurationsInMs.at(period) - timeInPeriodInMs;
        const auto restBitCountInPeriod = static_cast<size_t>(ThroughputsInKbps.at(period) * restTimeInPeriodInMs);
        const auto restBitCountToDownload = byteCount * CHAR_BIT - bitCount;
        if (restBitCountToDownload < restBitCountInPeriod) {
            const auto elapsedTimeInMs = static_cast<double>(restBitCountToDownload) / ThroughputsInKbps.at(period);
            timeInPeriodInMs += elapsedTimeInMs;
            data.TimeInMs += elapsedTimeInMs;
            bitCount = byteCount * CHAR_BIT;
        } else {
            period = period < DurationsInMs.size() - 1 ? period + 1 : 0, timeInPeriodInMs = 0;
            data.TimeInMs += restTimeInPeriodInMs;
            bitCount += restBitCountInPeriod;
        }
    }
    data.ByteCount = bitCount / CHAR_BIT;
    return data;
}

void NetworkModel::Delay(double timeInMs) {
    double elapsedTimeInMs = 0;
    while (elapsedTimeInMs < timeInMs) {
        const auto restTimeInPeriodInMs = DurationsInMs.at(period) - timeInPeriodInMs;
        if (timeInMs - elapsedTimeInMs < restTimeInPeriodInMs) {
            timeInPeriodInMs += timeInMs - elapsedTimeInMs;
            elapsedTimeInMs = timeInMs;
        } else {
            period = period < DurationsInMs.size() - 1 ? period + 1 : 0, timeInPeriodInMs = 0;
            elapsedTimeInMs += restTimeInPeriodInMs;
        }
    }
}
