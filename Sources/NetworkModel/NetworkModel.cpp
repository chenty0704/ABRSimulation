#include "Core/JSON.h"
#include "NetworkModel/NetworkModel.h"

NetworkModel NetworkModel::Import(std::string_view fileName) {
    const auto obj = JSON::Import(fileName).as_object();
    NetworkModel networkModel;
    networkModel.durationsInMs = boost::json::value_to<std::vector<int>>(obj.at("DurationsInMs"));
    networkModel.throughputsInKbps = boost::json::value_to<std::vector<int>>(obj.at("ThroughtputsInKbps"));
    return networkModel;
}

DownloadData NetworkModel::Download(int byteCount) {
    DownloadData data{.TimeInMs = 0, .ByteCount = 0};
    while (data.ByteCount < byteCount) {
        const auto restTimeInPeriodInMs = durationsInMs.at(period) - timeInPeriodInMs;
        const auto restBitCountInPeriod = throughputsInKbps.at(period) * restTimeInPeriodInMs;
        const auto restBitCountToDownload = (byteCount - data.ByteCount) * CHAR_BIT;
        if (restBitCountToDownload < restBitCountInPeriod) {
            const auto elapsedTimeInMs = restBitCountToDownload / throughputsInKbps.at(period);
            timeInPeriodInMs += elapsedTimeInMs;
            data.TimeInMs += elapsedTimeInMs;
            data.ByteCount = byteCount;
        } else {
            period = period < durationsInMs.size() - 1 ? period + 1 : 0;
            timeInPeriodInMs = 0;
            data.TimeInMs += restTimeInPeriodInMs;
            data.ByteCount += restBitCountInPeriod / CHAR_BIT;
        }
    }
    return data;
}

void NetworkModel::Delay(int timeInMs) {
    auto delayedTimeInMs = 0;
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
