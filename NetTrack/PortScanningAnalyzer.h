#pragma once
#include "IAnalyzer.hpp"
#include "PacketInfo.h"
#include <string>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>

struct ScanResult {
    std::string srcIp;
    std::string dstIp;
    int uniqueDstPorts;
    bool suspicious;
};

class PortScanningAnalyzer : public IAnalyzer
{
public:
    nlohmann::json analyze(const std::vector<PacketInfo>& packets) override;

private:
    std::vector<ScanResult> detectPortScanning(const std::vector<PacketInfo>& packets, int threshold);
};
