#pragma once
#include <pcap.h>
#include "Logger.h"

// прототип перехватчика пакетов
void packet_handler(
	u_char* param,
	const struct pcap_pkthdr* header,
	const u_char* pkt_data);

class PacketHandler
{
	pcap_if_t* alldevs;
	pcap_if_t* device;
	int inum;
	int deviceCount = 0;
	pcap_t* adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];

	PacketHandler();
};

