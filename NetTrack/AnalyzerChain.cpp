#include "AnalyzerChain.h"

void AnalyzerChain::addAnalyzer(std::unique_ptr<IAnalyzer> analyzer) {
    analyzers.push_back(std::move(analyzer));
}

nlohmann::json AnalyzerChain::runAll(const std::vector<PacketInfo> packets) {
    nlohmann::json report = nlohmann::json::array();

    for (auto& analyzer : analyzers) {
        report.push_back(analyzer->analyze(packets));
    }

    return report;
}

