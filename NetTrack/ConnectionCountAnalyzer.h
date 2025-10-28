#pragma once
#include "IAnalyzer.hpp"
#include <string>
#include <set>

class ConnectionCountAnalyzer : public IAnalyzer {
public:
    nlohmann::json analyze(const std::vector<PacketInfo>& packets) override;
};


