#pragma once
#include <vector>
#include <memory>

#include "IAnalyzer.hpp"

class AnalyzerChain {
private:
    std::vector<std::unique_ptr<IAnalyzer>> analyzers;

public:
    void addAnalyzer(std::unique_ptr<IAnalyzer> analyzer) {
        analyzers.push_back(std::move(analyzer));
    }

    nlohmann::json runAll(const std::vector<PacketInfo>& packets) {
        nlohmann::json report = nlohmann::json::array();

        for (auto& analyzer : analyzers) {
            report.push_back(analyzer->analyze(packets));
        }

        return report;
    }
};
