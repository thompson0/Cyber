import sys
import time
from scapy.all import Ether, IP, TCP, sendp

TARGET_IP = "192.168.15.20"  # IP do alvo
INTERFACE = "wlan0"          # Interface de rede
NUM_PACKETS = 100
DURATION = 5

def send_packets(target_ip, interface, num_packets, duration):
    end_time = time.time() + duration
    packet_count = 0

    while time.time() < end_time and packet_count < num_packets:
        packet = (
            Ether() /
            IP(dst=target_ip, src="192.0.2.123", id=1337) /  # IP spoof + ID customizado
            TCP(dport=80, flags="FA") /
            b"FIREWALL_CHAMPAO_TESTE"  # Payload visível no Wireshark
        )
        sendp(packet, iface=interface, verbose=False)
        packet_count += 1

if __name__ == "__main__":
    if sys.version_info[0] < 3:
        print("This script requires Python 3.")
        sys.exit(1)
    send_packets(TARGET_IP, INTERFACE, NUM_PACKETS, DURATION)
