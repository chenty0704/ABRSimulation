#include "Core/JSON.h"
#include "NetworkModel/NetworkModel.h"

NetworkModel NetworkModel::Import(std::string_view fileName) {
    const auto obj = JSON::Import(fileName).as_object();
    NetworkModel networkModel;
    networkModel.durationsInMs = boost::json::value_to<std::vector<size_t>>(obj.at("DurationsInMs"));
    networkModel.throughputsInKbps = boost::json::value_to<std::vector<size_t>>(obj.at("ThroughtputsInKbps"));
    return networkModel;
}

DownloadData NetworkModel::Download(size_t byteCount) {
    size_t bitCount = 0;
    DownloadData data{.TimeInMs = 0, .ByteCount = 0};
    while (bitCount < byteCount * CHAR_BIT) {
        const auto restTimeInPeriodInMs = durationsInMs.at(period) - timeInPeriodInMs;
        const auto restBitCountInPeriod = throughputsInKbps.at(period) * restTimeInPeriodInMs;
        const auto restBitCountToDownload = byteCount * CHAR_BIT - bitCount;
        if (restBitCountToDownload < restBitCountInPeriod) {
            const auto elapsedTimeInMs = restBitCountToDownload / throughputsInKbps.at(period);
            timeInPeriodInMs += elapsedTimeInMs;
            data.TimeInMs += elapsedTimeInMs;
            bitCount = byteCount * CHAR_BIT;
        } else {
            period = period < durationsInMs.size() - 1 ? period + 1 : 0;
            timeInPeriodInMs = 0;
            data.TimeInMs += restTimeInPeriodInMs;
            bitCount += restBitCountInPeriod;
        }
    }
    data.ByteCount = bitCount / CHAR_BIT;
    return data;
}

void NetworkModel::Delay(size_t timeInMs) {
    size_t delayedTimeInMs = 0;
    while (delayedTimeInMs < timeInMs) {
        const auto restTimeInPeriodInMs = durationsInMs.at(period) - timeInPeriodInMs;
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
