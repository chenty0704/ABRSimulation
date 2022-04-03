#include "Core/JSON.h"
#include "NetworkModel/NetworkModel.h"

NetworkModel NetworkModel::Import(std::string_view fileName) {
    const auto obj = JSON::Import(fileName).as_object();
    NetworkModel networkModel;
    networkModel.durationsInMs = boost::json::value_to<std::vector<double>>(obj.at("DurationsInMs"));
    networkModel.throughputsInKbps = boost::json::value_to<std::vector<double>>(obj.at("ThroughputsInKbps"));
    return networkModel;
}

DownloadData NetworkModel::Download(size_t byteCount) {
    size_t bitCount = 0;
    DownloadData data{0, 0};
    while (bitCount < byteCount * CHAR_BIT) {
        const auto restTimeInPeriodInMs = durationsInMs.at(period) - timeInPeriodInMs;
        const auto restBitCountInPeriod = static_cast<size_t>(throughputsInKbps.at(period) * restTimeInPeriodInMs);
        const auto restBitCountToDownload = byteCount * CHAR_BIT - bitCount;
        if (restBitCountToDownload < restBitCountInPeriod) {
            const auto elapsedTimeInMs = static_cast<double>(restBitCountToDownload) / throughputsInKbps.at(period);
            timeInPeriodInMs += elapsedTimeInMs;
            data.TimeInMs += elapsedTimeInMs;
            bitCount = byteCount * CHAR_BIT;
        } else {
            period = period < durationsInMs.size() - 1 ? period + 1 : 0, timeInPeriodInMs = 0;
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
        const auto restTimeInPeriodInMs = durationsInMs.at(period) - timeInPeriodInMs;
        if (timeInMs - elapsedTimeInMs < restTimeInPeriodInMs) {
            timeInPeriodInMs += timeInMs - elapsedTimeInMs;
            elapsedTimeInMs = timeInMs;
        } else {
            period = period < durationsInMs.size() - 1 ? period + 1 : 0, timeInPeriodInMs = 0;
            elapsedTimeInMs += restTimeInPeriodInMs;
        }
    }
}
