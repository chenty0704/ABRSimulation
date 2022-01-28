#include "NetworkModel.h"

DownloadData NetworkModel::Download(int bitCount) {
    DownloadData data{.TimeInMs = 0, .DownloadedBitCount = 0};
    while (data.DownloadedBitCount < bitCount) {
        const auto restTimeInPeriodInMs = durationsInMs[period] - timeInPeriodInMs;
        const auto restBitCountInPeriod = throughputsInKbps[period] * restTimeInPeriodInMs;
        if (bitCount - data.DownloadedBitCount < restBitCountInPeriod) {
            const auto elapsedTimeInMs = (bitCount - data.DownloadedBitCount) / throughputsInKbps[period];
            timeInPeriodInMs += elapsedTimeInMs;
            data.TimeInMs += elapsedTimeInMs;
            data.DownloadedBitCount = bitCount;
        } else {
            period = period < durationsInMs.size() - 1 ? period + 1 : 0;
            timeInPeriodInMs = 0;
            data.TimeInMs += restTimeInPeriodInMs;
            data.DownloadedBitCount += restBitCountInPeriod;
        }
    }
    return data;
}

void NetworkModel::Delay(int timeInMs) {
    auto delayedTimeInMs = 0;
    while (delayedTimeInMs < timeInMs) {
        const auto restTimeInPeriodInMs = durationsInMs[period] - timeInPeriodInMs;
        if (timeInMs - delayedTimeInMs < restTimeInPeriodInMs) {
            timeInPeriodInMs += timeInMs - delayedTimeInMs;
            delayedTimeInMs = timeInMs;
        } else {
            period = period < durationsInMs.size() - 1 ? period + 1 : 0;
            timeInPeriodInMs = 0;
            delayedTimeInMs += restTimeInPeriodInMs;
        }
    }
}
