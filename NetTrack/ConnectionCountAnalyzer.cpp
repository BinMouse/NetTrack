#include "ConnectionCountAnalyzer.h"
#include "Report.hpp"
nlohmann::json ConnectionCountAnalyzer::analyze(const std::vector<PacketInfo>& packets) {
    int threshold = 100;
    Report result("connCount");
    //nlohmann::json result;
    //result["rule"] = "connCount";

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

    return result.getJson();
}