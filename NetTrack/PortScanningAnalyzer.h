#pragma once
#include "IAnalyzer.hpp"
#include <string>
#include <unordered_map>
#include <set>
#include <unordered_set>

class PortScanningAnalyzer :
    public IAnalyzer
{
public:
    nlohmann::json analyze(const std::vector<PacketInfo>& packets) override;
};

