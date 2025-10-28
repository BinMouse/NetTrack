#pragma once
#include <vector>
#include <memory>

#include "IAnalyzer.hpp"

class AnalyzerChain {
private:
    std::vector<std::unique_ptr<IAnalyzer>> analyzers;

public:
    void addAnalyzer(std::unique_ptr<IAnalyzer> analyzer);

    nlohmann::json runAll(const std::vector<PacketInfo> packets);
};
