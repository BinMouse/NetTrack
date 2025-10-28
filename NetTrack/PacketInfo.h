#pragma once
#include <string>
struct PacketInfo {
    std::string srcIp;
    std::string dstIp;
    std::string protocol;
    int srcPort;
    int dstPort;
    int payloadLen;
    int count;

    bool operator==(const PacketInfo& other) const;

    bool operator!=(const PacketInfo& other) const;
};
