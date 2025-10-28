#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include <filesystem>
#include <list>
#include <thread>
#include "windivert.h"

#include "Settings.h"
#include "PacketInfo.h"
#include "AnalyzerChain.h"
#include "ConnectionCountAnalyzer.h"

using json = nlohmann::json;

#define MAX_PACKETS 50000

DWORD WINAPI ShowSettingsWindow(LPVOID);



void to_json(json& j, const PacketInfo& p) {
    j = json{
        {"count", p.count},
        {"src_port", p.srcPort},
        {"dst_port", p.dstPort},
        {"src_ip", p.srcIp},
        {"dst_ip", p.dstIp},
        {"payloadLen", p.payloadLen }
    };
}

/// <summary>
/// Логирование данных о пакете
/// </summary>
PacketInfo LogPacketInfo(PVOID packet, UINT packetLen) {
    PWINDIVERT_IPHDR ipHdr = nullptr;
    PWINDIVERT_IPV6HDR ipv6Hdr = nullptr;
    UINT8 protocol = 0;
    PWINDIVERT_TCPHDR tcpHdr = nullptr;
    PWINDIVERT_UDPHDR udpHdr = nullptr;
    PWINDIVERT_ICMPHDR IcmpHdr = nullptr;
    PWINDIVERT_ICMPV6HDR Icmpv6Hdr = nullptr;
    PVOID payload = nullptr;
    UINT payloadLen = 0;

    PacketInfo pInfo;
    pInfo.protocol = "unknown";
    pInfo.srcPort = 0;
    pInfo.dstPort = 0;
    pInfo.payloadLen = 0;

    BOOL parsed = WinDivertHelperParsePacket(
        packet,
        packetLen,
        &ipHdr,
        &ipv6Hdr,
        &protocol,
        &IcmpHdr,
        &Icmpv6Hdr,
        &tcpHdr,
        &udpHdr,
        &payload,
        &payloadLen,
        nullptr,
        nullptr
    );

    if (!parsed) {
        std::cerr << "[!] Failed to parse packet.\n";
        return pInfo;
    }

    std::cerr << "\n----------------\n";

    // IPv4
    if (ipHdr) {
        pInfo.protocol = "IPv4";

        char srcStr[16], dstStr[16];
        snprintf(srcStr, sizeof(srcStr), "%u.%u.%u.%u",
            ipHdr->SrcAddr & 0xFF,
            (ipHdr->SrcAddr >> 8) & 0xFF,
            (ipHdr->SrcAddr >> 16) & 0xFF,
            (ipHdr->SrcAddr >> 24) & 0xFF);
        snprintf(dstStr, sizeof(dstStr), "%u.%u.%u.%u",
            ipHdr->DstAddr & 0xFF,
            (ipHdr->DstAddr >> 8) & 0xFF,
            (ipHdr->DstAddr >> 16) & 0xFF,
            (ipHdr->DstAddr >> 24) & 0xFF);

        pInfo.srcIp = srcStr;
        pInfo.dstIp = dstStr;

        std::cerr << "IPv4: " << srcStr << " -> " << dstStr << "\n";
    }

    // IPv6
    else if (ipv6Hdr) {
        pInfo.protocol = "IPv6";

        char srcStr[INET6_ADDRSTRLEN] = { 0 };
        char dstStr[INET6_ADDRSTRLEN] = { 0 };

        IN6_ADDR in6_src;
        IN6_ADDR in6_dst;

        memcpy(&in6_src.u.Byte, ipv6Hdr->SrcAddr, 16);
        memcpy(&in6_dst.u.Byte, ipv6Hdr->DstAddr, 16);

        if (!InetNtopA(AF_INET6, &in6_src, srcStr, INET6_ADDRSTRLEN)) {
            strcpy_s(srcStr, "InvalidIPv6");
        }
        if (!InetNtopA(AF_INET6, &in6_dst, dstStr, INET6_ADDRSTRLEN)) {
            strcpy_s(dstStr, "InvalidIPv6");
        }

        pInfo.srcIp = srcStr;
        pInfo.dstIp = dstStr;

        std::cerr << "IPv6: " << srcStr << " -> " << dstStr << "\n";
    }

    // TCP
    if (tcpHdr) {
        pInfo.protocol = "TCP";
        pInfo.srcPort = ntohs(tcpHdr->SrcPort);
        pInfo.dstPort = ntohs(tcpHdr->DstPort);
        pInfo.payloadLen = payloadLen;

        std::cerr << "TCP srcPort=" << pInfo.srcPort
            << " dstPort=" << pInfo.dstPort
            << " payloadLen=" << payloadLen << "\n";
    }

    // UDP
    else if (udpHdr) {
        pInfo.protocol = "UDP";
        pInfo.srcPort = ntohs(udpHdr->SrcPort);
        pInfo.dstPort = ntohs(udpHdr->DstPort);
        pInfo.payloadLen = payloadLen;

        std::cerr << "UDP srcPort=" << pInfo.srcPort
            << " dstPort=" << pInfo.dstPort
            << " payloadLen=" << payloadLen << "\n";
    }

    // ICMP / ICMPv6
    else if (IcmpHdr || Icmpv6Hdr) {
        pInfo.protocol = IcmpHdr ? "ICMP" : "ICMPv6";
        std::cerr << pInfo.protocol << " packet detected\n";
    }

    // Если ничего не определено
    if (pInfo.protocol == "unknown")
        std::cerr << "[!] Unknown packet type.\n";

    return pInfo;
}

/// <summary>
/// Добавление пакета в массив без дубликатов
/// </summary>
void writePacketInfoToLog(PacketInfo packet, PacketInfo* FlowLog, size_t& logCount) {
    // Проверяем, есть ли уже такой пакет
    for (size_t i = 0; i < logCount; ++i) {
        if (FlowLog[i] == packet) {
            FlowLog[i].count++;
            return;
        }
    }

    if (logCount < MAX_PACKETS) {
        FlowLog[logCount] = packet;
        FlowLog[logCount].count = 1;
        ++logCount;
    }
}

/// <summary>
/// Вывод ошибки WinDivert в консоль
/// </summary>
/// <param name="msg"></param>
void PrintLastError(const char* msg) {
    DWORD err = GetLastError();
    LPSTR buf = nullptr;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&buf, 0, NULL);
    std::cerr << msg << " (GetLastError=" << err << "): " << (buf ? buf : "Unknown") << std::endl;
    if (buf) LocalFree(buf);
}


/// <summary>
/// Сохранение лога в файл log.json
/// </summary>
void LogOutput(PacketInfo* FlowLog, size_t logCount, AnalyzerChain& analyzerChain) {
    if (logCount == 0) return;

    // 1. Преобразуем FlowLog в std::vector для удобства
    std::vector<PacketInfo> packets(FlowLog, FlowLog + logCount);

    // 2. JSON лог пакетов
    nlohmann::json logJSON = nlohmann::json::array();
    for (const auto& p : packets) {
        logJSON.push_back(p);
    }

    // 3. Генерируем отчёт анализаторов
    nlohmann::json reportJSON = analyzerChain.runAll(packets);

    // 4. Собираем всё в один объект
    nlohmann::json finalJSON;
    finalJSON["log"] = logJSON;
    finalJSON["report"] = reportJSON;

    // 5. Формируем имя файла по дате и времени
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t filename[128];
    swprintf(filename, 128, L"%04d%02d%02d%02d%02d%02d.json",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    std::filesystem::path fullPath = std::filesystem::path(g_settings.logPath) / filename;
    std::filesystem::create_directories(g_settings.logPath);

    // 6. Сохраняем JSON
    std::ofstream oFile(fullPath, std::ios::out | std::ios::trunc);
    if (oFile.is_open()) {
        oFile << finalJSON.dump(1); // красивый вывод с отступами
        oFile.close();
        std::wcout << L"[+] Log and report saved to " << fullPath.wstring() << L"\n";
    }
    else {
        std::wcerr << L"[!] Failed to save log to " << fullPath.wstring() << L"\n";
    }
}

int main() {
    const int BUFSIZE = 65535;
    char packet[BUFSIZE];
    UINT packetLen = 0;
    WINDIVERT_ADDRESS addr;

    CreateThread(nullptr, 0, ShowSettingsWindow, nullptr, 0, nullptr);

    PacketInfo* FlowLog = new PacketInfo[MAX_PACKETS];
    size_t logCount = 0;

    HANDLE handle = WinDivertOpen("true", WINDIVERT_LAYER_NETWORK, 0, 0);
    if (handle == INVALID_HANDLE_VALUE) {
        PrintLastError("WinDivertOpen failed");
        return 1;
    }
    std::cerr << "WinDivert opened. Ready to capture packets. Press Ctrl-C to stop.\n";

    DWORD64 lastSaveTime = GetTickCount64();

    while (true) {
        if (!WinDivertRecv(handle, packet, BUFSIZE, &packetLen, &addr)) {
            std::cerr << "WinDivertRecv failed\n";
            break;
        }

        PacketInfo p = LogPacketInfo(packet, packetLen);
        writePacketInfoToLog(p, FlowLog, logCount);

        if (!WinDivertSend(handle, packet, packetLen, NULL, &addr)) {
            std::cerr << "WinDivertSend failed\n";
            break;
        }

        DWORD64 now = GetTickCount64();
        if (now - lastSaveTime >= g_settings.saveIntervalMinutes * 60 * 1000 || logCount == MAX_PACKETS) {
            AnalyzerChain analyzerChain;
            analyzerChain.addAnalyzer(std::make_unique<ConnectionCountAnalyzer>());

            LogOutput(FlowLog, logCount, analyzerChain);
            logCount = 0;
            lastSaveTime = now;
        }
    }
    WinDivertClose(handle);
    delete[] FlowLog;
    return 0;
}