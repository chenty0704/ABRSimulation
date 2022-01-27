#include "NetworkModel.h"

DownloadData NetworkModel::Download(int byteCount) {
    DownloadData data{.TimeInMs = 0, .DownloadedByteCount = 0};
    while (data.DownloadedByteCount < byteCount) {
        const auto restTimeInPeriodInMs = durationsInMs[period] - timeInPeriodInMs;
        const auto restByteCapacityInPeriod = bandwidthsInKBps[period] * restTimeInPeriodInMs;
        if (byteCount - data.DownloadedByteCount < restByteCapacityInPeriod) {
            const auto elapsedTimeInMs = (byteCount - data.DownloadedByteCount) / bandwidthsInKBps[period];
            timeInPeriodInMs += elapsedTimeInMs;
            data.TimeInMs += elapsedTimeInMs;
            data.DownloadedByteCount = byteCount;
        } else {
            period = period < durationsInMs.size() - 1 ? period + 1 : 0;
            timeInPeriodInMs = 0;
            data.TimeInMs += restTimeInPeriodInMs;
            data.DownloadedByteCount += restByteCapacityInPeriod;
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
