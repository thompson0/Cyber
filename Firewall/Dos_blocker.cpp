#include <pcap.h>
#include <iostream>
#include <unordered_map>
#include <set>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define THRESHOLD 40

std::unordered_map<std::string, int> packet_count;
std::set<std::string> blocked_ips;
std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

void block_ip(const std::string& ip) {
    if (blocked_ips.find(ip) == blocked_ips.end()) {
        std::string cmd = "iptables -A INPUT -s " + ip + " -j DROP";
        std::system(cmd.c_str());
        std::cout << "[!] Bloqueando IP: " << ip << std::endl;
        blocked_ips.insert(ip);
    }
}

void packet_handler(u_char* , const struct pcap_pkthdr*, const u_char* packet) {
    const struct ip* ip_header = (struct ip*)(packet + 14); // Ethernet header = 14 bytes
    std::string src_ip = inet_ntoa(ip_header->ip_src);
    packet_count[src_ip]++;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();

    if (elapsed >= 1) {
        for (const auto& entry : packet_count) {
            if (entry.second > THRESHOLD) {
                block_ip(entry.first);
            }
        }
        packet_count.clear();
        start_time = now;
    }
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t* alldevs;
    pcap_findalldevs(&alldevs, errbuf);

    if (!alldevs) {
        std::cerr << "Erro ao listar interfaces: " << errbuf << std::endl;
        return 1;
    }

    std::string iface = alldevs->name;
    std::cout << "[*] Usando interface: " << iface << std::endl;
    pcap_freealldevs(alldevs);

    pcap_t* handle = pcap_open_live(iface.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (!handle) {
        std::cerr << "Erro ao abrir a interface: " << errbuf << std::endl;
        return 1;
    }

    std::cout << "[*] Monitorando pacotes IP..." << std::endl;

    pcap_loop(handle, 0, packet_handler, nullptr);
    pcap_close(handle);

    return 0;
}
