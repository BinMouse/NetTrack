#include "ConnectionCountAnalyzer.h"
nlohmann::json ConnectionCountAnalyzer::analyze(const std::vector<PacketInfo>& packets) {
    int threshold = 100;
    nlohmann::json result;
    result["rule"] = "connCnt";

    std::set<std::string> suspiciousIps;

    std::map<std::string, int> connCount;
    for (const auto& p : packets) {
        if (p.count > threshold) {
            suspiciousIps.insert(p.srcIp + "->" + p.dstIp);
        }
    }

    result["suspicious"] = nlohmann::json::array();
    for (const auto& ip : suspiciousIps)
        result["suspicious"].push_back(ip);

    // Даже если ничего не найдено
    if (suspiciousIps.empty())
        result["suspicious"] = nlohmann::json::array();

    return result;
}