#include "PacketInfo.h"
bool PacketInfo::operator==(const PacketInfo& other) const {
    return srcIp == other.srcIp && dstIp == other.dstIp
        && srcPort == other.srcPort && dstPort == other.dstPort
        && protocol == other.protocol;
}

bool PacketInfo::operator!=(const PacketInfo& other) const {
    return !(*this == other);
}