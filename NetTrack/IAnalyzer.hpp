#pragma once
#include <vector>
#include <nlohmann/json.hpp>
#include "PacketInfo.h"
class IAnalyzer {
public:
    virtual ~IAnalyzer() = default;
    virtual nlohmann::json analyze(const std::vector<PacketInfo>& packets) = 0;
};
