#include "PortScanningAnalyzer.h"
nlohmann::json PortScanningAnalyzer::analyze(const std::vector<PacketInfo>& packets) {
    nlohmann::json result;
    std::set<std::string> suspiciousIps;
    const int threshold = 100;
    result["rule"] = "portScan";

    std::unordered_map<std::string, std::unordered_set<int>> portsBySrc;

    for (const auto& p : packets) {
        portsBySrc[p.srcIp].insert(p.dstPort);
    }

    nlohmann::json counts = nlohmann::json::object();

    for (const auto& [src, ports] : portsBySrc) {
        counts[src] = ports.size();
        if (ports.size() > threshold) {
            suspiciousIps.insert(src);
        }
    }

    result["dst_ports_count"] = counts;
    result["suspicious_ips"] = suspiciousIps;
    result["threshold"] = threshold;

    return result;
}