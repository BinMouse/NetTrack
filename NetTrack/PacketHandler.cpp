#include "PacketHandler.h"

PacketHandler::PacketHandler()
{
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING,
		NULL, &alldevs, errbuf) == -1)
	{
        Logger::getInstance().log(std::string("Error in pcap_findalldevs: ") + errbuf);
		exit(1);
	}

    deviceCount = 0;
    std::string devicesList;

    for (device = alldevs; device; device = device->next)
    {
        std::string desc = (device->description) ? device->description : "No description available";
        std::string line = std::to_string(++deviceCount) + ". " + device->name + " (" + desc + ")";

        devicesList += line + "\n";
    }

    Logger::getInstance().log(devicesList);
}