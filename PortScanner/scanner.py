import socket
import threading
import argparse

def scan_port(ip, port):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(1)
        result = sock.connect_ex((ip, port))
        if result == 0:
            try:
                sock.send(b'Hello\r\n')
                banner = sock.recv(1024).decode().strip()
            except:
                banner = 'No banner'
            print(f'[+] Porta {port} aberta - {banner}')
        sock.close()
    except Exception as e:
        print(f'[-] Erro ao escanear a porta {port}: {e}')
        
def main():
    parser = argparse.ArgumentParser(description='Scanner de Porta Simples')
    parser.add_argument('ip', help="Endereço IP ou domínio")
    parser.add_argument('-p', '--ports', help="Portas a escanear (ex: 20-80 ou all)", default="1-1024")
    args = parser.parse_args()

    ip = args.ip

    # Processa o argumento de porta
    if args.ports.lower() == 'all':
        port_range = range(0, 65536)
    else:
        try:
            start_port, end_port = map(int, args.ports.split('-'))
            port_range = range(start_port, end_port + 1)
        except:
            print("Formato inválido para -p. Use algo como 20-80 ou 'all'.")
            exit()

    print(f'[*] Escaneando {ip} da porta {port_range.start} até {port_range.stop - 1}')

    threads = []
    for port in port_range:
        t = threading.Thread(target=scan_port, args=(ip, port))
        threads.append(t)
        t.start()

    for t in threads:
        t.join()

if __name__ == "__main__":
    main()
