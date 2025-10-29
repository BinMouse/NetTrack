#include "PortScanningAnalyzer.h"
#include "PortScanningAnalyzer.h"
#include <nlohmann/json.hpp>

nlohmann::json PortScanningAnalyzer::analyze(const std::vector<PacketInfo>& packets) {
    nlohmann::json result;
    result["rule"] = "portScan";

    const int threshold = 100;

    std::vector<ScanResult> scanResults = detectPortScanning(packets, threshold);

    // dst_ports_count è suspicious_ips
    nlohmann::json counts = nlohmann::json::object();
    std::set<std::string> suspiciousIps;

    for (const auto& r : scanResults) {
        counts[r.srcIp] = counts.contains(r.srcIp) ? counts[r.srcIp].get<int>() + r.uniqueDstPorts : r.uniqueDstPorts;

        if (r.suspicious) {
            suspiciousIps.insert(r.srcIp);
        }
    }

    result["dst_ports_count"] = counts;
    result["suspicious_ips"] = suspiciousIps;
    result["threshold"] = threshold;

    return result;
}


std::vector<ScanResult> PortScanningAnalyzer::detectPortScanning(const std::vector<PacketInfo>& packets, int threshold) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::set<int>>> portsMap;

    for (const auto& p : packets) {
        if (p.protocol == "TCP" || p.protocol == "UDP") {
            portsMap[p.srcIp][p.dstIp].insert(p.dstPort);
        }
    }

    std::vector<ScanResult> results;

    for (const auto& [srcIp, dstMap] : portsMap) {
        for (const auto& [dstIp, portSet] : dstMap) {
            ScanResult r;
            r.srcIp = srcIp;
            r.dstIp = dstIp;
            r.uniqueDstPorts = static_cast<int>(portSet.size());
            r.suspicious = r.uniqueDstPorts > threshold;
            results.push_back(r);
        }
    }

    return results;
}