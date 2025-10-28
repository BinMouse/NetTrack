#include "ConnectionCountAnalyzer.h"
nlohmann::json ConnectionCountAnalyzer::analyze(const std::vector<PacketInfo>& packets) {
    nlohmann::json result;
    result["rule"] = "connCnt";

    std::set<std::string> suspiciousIps;

    std::map<std::string, int> connCount;
    for (const auto& p : packets) {
        connCount[p.srcIp]++;
        if (connCount[p.srcIp] > 100) {
            suspiciousIps.insert(p.srcIp);
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