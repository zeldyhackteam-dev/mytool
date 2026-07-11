<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Valex | Digital Security Architect</title>
    <meta name="description" content="Valex-Grand - Cybersecurity Researcher and Lead Developer of Leviathan & Aegis-Gate.">
    
    <!-- 🚀 BARIYI VE LINKEDIN'I ZİNCİRLEYEN GOOGLE ŞEMA VERİSİ (SEO) -->
    <script type="application/ld+json">
    {
      "@context": "https://schema.org",
      "@type": "Person",
      "name": "Valex Grand",
      "url": "https://valex-grand.github.io/",
      "sameAs": [
        "https://www.linkedin.com/in/valex-grand",
        "https://medium.com/@valexgrand"
      ],
      "jobTitle": "Cyber Security Researcher & Software Developer"
    }
    </script>
    
    <style>
        :root {
            --neon-green: #00ff41;
            --darker-bg: #05080a;
            --glass-bg: rgba(10, 15, 20, 0.9);
            --border-glow: 0 0 15px rgba(0, 255, 65, 0.3);
        }

        * { margin: 0; padding: 0; box-sizing: border-box; cursor: crosshair; }

        body {
            background-color: var(--darker-bg);
            color: #e0e0e0;
            font-family: 'Fira Code', 'Courier New', monospace;
            overflow-x: hidden;
            line-height: 1.6;
        }

        /* Matrix Arka Plan Efekti */
        #canvas {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            z-index: -1;
            opacity: 0.15;
        }

        .main-frame {
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }

        .terminal {
            width: 100%;
            max-width: 900px;
            background: var(--glass-bg);
            border: 1px solid var(--neon-green);
            box-shadow: var(--border-glow);
            border-radius: 8px;
            backdrop-filter: blur(10px);
            overflow: hidden;
            animation: fadeIn 1.5s ease-in-out;
        }

        @keyframes fadeIn { from { opacity: 0; transform: translateY(20px); } to { opacity: 1; transform: translateY(0); } }

        .terminal-header {
            background: rgba(0, 255, 65, 0.1);
            padding: 12px 20px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-bottom: 1px solid rgba(0, 255, 65, 0.2);
        }

        .dots { display: flex; gap: 8px; }
        .dot { width: 12px; height: 12px; border-radius: 50%; }
        .red { background: #ff5f56; }
        .yellow { background: #ffbd2e; }
        .green { background: #27c93f; }

        .content { padding: 40px; }

        .glitch {
            font-size: 2.5rem;
            font-weight: bold;
            color: var(--neon-green);
            position: relative;
            text-shadow: 2px 2px #ff000033, -2px -2px #0000ff33;
            margin-bottom: 10px;
        }

        .typewriter {
            font-size: 1.1rem;
            color: #888;
            margin-bottom: 30px;
            border-right: 2px solid var(--neon-green);
            width: fit-content;
            white-space: nowrap;
            overflow: hidden;
            animation: typing 3s steps(30, end), blink .75s step-end infinite;
        }

        @keyframes typing { from { width: 0 } to { width: 100% } }
        @keyframes blink { from, to { border-color: transparent } 50% { border-color: var(--neon-green); } }

        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
            gap: 20px;
            margin-top: 30px;
        }

        .card {
            border: 1px solid rgba(0, 255, 65, 0.1);
            padding: 20px;
            background: rgba(255, 255, 255, 0.02);
            border-radius: 4px;
            transition: all 0.4s ease;
            position: relative;
            overflow: hidden;
        }

        .card:hover {
            border-color: var(--neon-green);
            transform: scale(1.03);
            background: rgba(0, 255, 65, 0.05);
            box-shadow: 0 0 15px rgba(0, 255, 65, 0.1);
        }

        .card h3 { color: var(--neon-green); margin-bottom: 10px; font-size: 1.2rem; }
        .card p { font-size: 0.9rem; color: #aaa; }

        .social-links {
            margin-top: 40px;
            display: flex;
            gap: 25px;
            flex-wrap: wrap;
        }

        .social-links a {
            color: #fff;
            text-decoration: none;
            font-size: 0.9rem;
            display: flex;
            align-items: center;
            gap: 8px;
            transition: 0.3s;
            border-bottom: 1px solid transparent;
        }

        .social-links a:hover {
            color: var(--neon-green);
            border-bottom: 1px solid var(--neon-green);
        }

        .footer {
            text-align: center;
            margin-top: 40px;
            font-size: 0.7rem;
            color: #444;
            letter-spacing: 2px;
        }
    </style>
</head>
<body>

    <canvas id="canvas"></canvas>

    <div class="main-frame">
        <div class="terminal">
            <div class="terminal-header">
                <div class="dots">
                    <div class="dot red"></div>
                    <div class="dot yellow"></div>
                    <div class="dot green"></div>
                </div>
                <div style="font-size: 0.7rem; color: var(--neon-green); opacity: 0.7;">ROOT@VALEX_CORE: ~</div>
            </div>

            <div class="content">
                <div class="glitch">VALEX-GRAND</div>
                <div class="typewriter">System Security Researcher & Developer</div>
                
                <p style="color: #ccc; max-width: 600px;">
                    Düşük seviyeli sistem mimarileri, veri parçalama algoritmaları ve ağ güvenliği üzerine uzmanlaşıyorum. 
                    <span style="color: var(--neon-green);">Rust, Go ve Python</span> ile defansif teknolojiler geliştiriyorum.
                </p>

                <div class="grid">
                    <div class="card">
                        <h3>⚡ Leviathan</h3>
                        <p>Dosya parçalama ve hibrit şifreleme motoru. Veriyi 100 parçaya bölerek güvenliği maksimize eder.</p>
                    </div>
                    <div class="card">
                        <h3>🛡️ Aegis-Gate</h3>
                        <p>Gerçek zamanlı trafik analizi yapan Rust tabanlı network kalkanı.</p>
                    </div>
                    <div class="card">
                        <h3>👁️ Aegis-Eye</h3>
                        <p>Dağıtık ağ izleme ve anomali tespit sistemi. Next.js dashboard entegreli.</p>
                    </div>
                </div>

                <!-- 🔗 BOTLARIN TAKİP EDECEĞİ GÜVENLİ SOSYAL BAĞLANTILAR -->
                <div class="social-links">
                    <a href="https://github.com/Valex-Grand" rel="noopener" target="_blank">/GITHUB</a>
                    <a href="https://medium.com/@valexgrand" rel="noopener" target="_blank">/MEDIUM</a>
                    <a href="https://www.linkedin.com/in/valex-grand" rel="noopener" target="_blank">/LINKEDIN</a>
                    <a href="https://instagram.com/valexgrand" style="opacity: 0.6;" rel="noopener" target="_blank">/INSTAGRAM</a>
                </div>

                <div class="footer">
                    [ SYSTEMS OPERATIONAL | VERIFICATION: 2026 ]
                </div>
            </div>
        </div>
    </div>

    <script>
        const canvas = document.getElementById('canvas');
        const ctx = canvas.getContext('2d');

        canvas.height = window.innerHeight;
        canvas.width = window.innerWidth;

        const characters = "01ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#$%^&*()*&^%";
        const fontSize = 12;
        const columns = canvas.width / fontSize;
        const drops = [];

        for (let x = 0; x < columns; x++) drops[x] = 1;

        function draw() {
            ctx.fillStyle = 'rgba(5, 8, 10, 0.05)';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            ctx.fillStyle = '#00ff41';
            ctx.font = fontSize + 'px monospace';

            for (let i = 0; i < drops.length; i++) {
                const text = characters.charAt(Math.floor(Math.random() * characters.length));
                ctx.fillText(text, i * fontSize, drops[i] * fontSize);
                if (drops[i] * fontSize > canvas.height && Math.random() > 0.975) drops[i] = 0;
                if (window.innerHeight < canvas.height) { canvas.height = window.innerHeight; }
                drops[i]++;
            }
        }
        setInterval(draw, 33);

        window.addEventListener('resize', () => {
            canvas.height = window.innerHeight;
            canvas.width = window.innerWidth;
        });
    </script>
</body>
</html>
