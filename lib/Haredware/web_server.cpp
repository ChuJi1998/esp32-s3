#include "web_server.h"
#include "relay.h"
#include <WiFi.h>

/* ── 全局 WebServer 对象 ── */
WebServer server(80);

/* ── 继电器状态记录（12路，内存中跟踪） ── */
enum RelayState { RELAY_OFF, RELAY_ON, RELAY_BLINK };
static RelayState relayState[12] = { RELAY_OFF };

/* ── 函数指针表：方便按索引调用 ── */
typedef void (*RelayFunc)(void);
static const RelayFunc relayOn[12] = {
  Relay1_on, Relay2_on, Relay3_on, Relay4_on,
  Relay5_on, Relay6_on, Relay7_on, Relay8_on,
  Relay9_on, Relay10_on, Relay11_on, Relay12_on
};
static const RelayFunc relayOff[12] = {
  Relay1_off, Relay2_off, Relay3_off, Relay4_off,
  Relay5_off, Relay6_off, Relay7_off, Relay8_off,
  Relay9_off, Relay10_off, Relay11_off, Relay12_off
};
static const RelayFunc relayBlink[12] = {
  Relay1_blink, Relay2_blink, Relay3_blink, Relay4_blink,
  Relay5_blink, Relay6_blink, Relay7_blink, Relay8_blink,
  Relay9_blink, Relay10_blink, Relay11_blink, Relay12_blink
};

/* ════════════════════════════════════════════
   HTML 页面（优美的 Web 控制界面）
   ════════════════════════════════════════════ */
static const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>继电器控制系统</title>
<style>
  :root {
    --bg: #0f172a; --card: #1e293b; --border: #334155;
    --on: #22c55e; --off: #ef4444; --blink: #f59e0b;
    --text: #e2e8f0; --muted: #94a3b8; --accent: #3b82f6;
  }
  * { margin:0; padding:0; box-sizing:border-box; }
  body {
    font-family:'Segoe UI',system-ui,sans-serif;
    background:var(--bg); color:var(--text);
    min-height:100vh; padding:16px;
  }
  .header {
    text-align:center; padding:24px 0;
    background:linear-gradient(135deg, #1e293b 0%, #0f172a 100%);
    border-radius:12px; margin-bottom:20px;
    border:1px solid var(--border);
  }
  .header h1 { font-size:1.6rem; font-weight:700; letter-spacing:-0.5px; }
  .header .ip {
    font-size:0.85rem; color:var(--accent); margin-top:6px;
    font-family:'Cascadia Code',monospace;
  }
  .grid {
    display:grid;
    grid-template-columns:repeat(auto-fill, minmax(140px,1fr));
    gap:12px;
  }
  .card {
    background:var(--card); border:1px solid var(--border);
    border-radius:10px; padding:14px; text-align:center;
    transition:all 0.2s ease;
  }
  .card:hover { border-color:var(--accent); transform:translateY(-2px); }
  .card .num {
    font-size:0.75rem; color:var(--muted); text-transform:uppercase;
    letter-spacing:1px; margin-bottom:6px;
  }
  .card .status {
    display:inline-block; width:10px; height:10px; border-radius:50%;
    margin-bottom:8px; transition:background 0.3s;
  }
  .card .status.off { background:var(--off); box-shadow:0 0 8px var(--off); }
  .card .status.on  { background:var(--on);  box-shadow:0 0 8px var(--on); }
  .card .status.blink { background:var(--blink); box-shadow:0 0 8px var(--blink); animation:pulse 0.5s infinite; }
  @keyframes pulse { 0%,100%{opacity:1} 50%{opacity:0.3} }
  .card .label {
    font-size:0.8rem; font-weight:600; margin-bottom:10px; display:block;
  }
  .btn-group { display:flex; gap:4px; justify-content:center; flex-wrap:wrap; }
  button {
    border:none; border-radius:6px; padding:6px 10px; font-size:0.7rem;
    font-weight:600; cursor:pointer; transition:all 0.15s;
    color:#fff; letter-spacing:0.3px;
  }
  button:active { transform:scale(0.95); }
  .btn-on  { background:var(--on); }
  .btn-off { background:var(--off); }
  .btn-blink { background:var(--blink); }
  button:hover { filter:brightness(1.2); }
  .toast {
    position:fixed; top:16px; right:16px; padding:10px 20px;
    border-radius:8px; font-size:0.85rem; color:#fff; z-index:999;
    opacity:0; transform:translateX(40px);
    transition:all 0.3s ease; pointer-events:none;
  }
  .toast.show { opacity:1; transform:translateX(0); }
  .toast.success { background:#22c55e; }
  .toast.error { background:#ef4444; }
  footer {
    text-align:center; margin-top:24px; font-size:0.7rem;
    color:var(--muted);
  }
</style>
</head>
<body>

<div class="header">
  <h1>&#9889; 继电器控制系统</h1>
  <div class="ip" id="ipInfo">IP: --</div>
</div>

<div class="grid" id="relayGrid"></div>

<div class="toast" id="toast"></div>

<footer>ESP32-S3 &middot; 12路RS485继电器</footer>

<script>
const TOTAL = 12;
const grid = document.getElementById('relayGrid');
const toast = document.getElementById('toast');

// 构建卡片
for (let i = 1; i <= TOTAL; i++) {
  const card = document.createElement('div');
  card.className = 'card';
  card.innerHTML = `
    <div class="num">通道 ${i}</div>
    <div class="status off" id="dot${i}"></div>
    <span class="label" id="label${i}">OFF</span>
    <div class="btn-group">
      <button class="btn-on" onclick="sendCmd(${i},'on')">ON</button>
      <button class="btn-off" onclick="sendCmd(${i},'off')">OFF</button>
      <button class="btn-blink" onclick="sendCmd(${i},'blink')">BLINK</button>
    </div>`;
  grid.appendChild(card);
}

// 发送指令
function sendCmd(id, cmd) {
  fetch('/api/' + cmd + '?id=' + id)
    .then(r => r.json())
    .then(data => {
      if (data.ok) {
        updateUI(id, cmd);
        showToast('通道 ' + id + ' : ' + cmd.toUpperCase(), 'success');
      } else {
        showToast('操作失败', 'error');
      }
    })
    .catch(() => showToast('请求失败', 'error'));
}

// 更新UI状态
function updateUI(id, cmd) {
  const dot = document.getElementById('dot' + id);
  const label = document.getElementById('label' + id);
  dot.className = 'status ' + cmd;
  if (cmd === 'on') {
    label.textContent = 'ON';
    label.style.color = '#22c55e';
  } else if (cmd === 'off') {
    label.textContent = 'OFF';
    label.style.color = '#94a3b8';
  } else {
    label.textContent = 'BLINK';
    label.style.color = '#f59e0b';
  }
}

// Toast提示
let toastTimer;
function showToast(msg, type) {
  clearTimeout(toastTimer);
  toast.textContent = msg;
  toast.className = 'toast ' + type + ' show';
  toastTimer = setTimeout(() => toast.classList.remove('show'), 1800);
}

// 定时刷新状态
function refreshStatus() {
  fetch('/api/status')
    .then(r => r.json())
    .then(data => {
      if (data.ip) {
        document.getElementById('ipInfo').textContent = 'IP: ' + data.ip;
      }
      for (let i = 1; i <= TOTAL; i++) {
        const st = data.states[i - 1];
        updateUI(i, st === 1 ? 'on' : st === 2 ? 'blink' : 'off');
      }
    });
}
setInterval(refreshStatus, 2000);
refreshStatus();
</script>
</body>
</html>
)rawliteral";

/* ════════════════════════════════════════════
   API 处理函数
   ════════════════════════════════════════════ */

static void handleApiOn() {
  int id = server.arg("id").toInt();
  if (id >= 1 && id <= 12) {
    relayOn[id - 1]();
    relayState[id - 1] = RELAY_ON;
    server.send(200, "application/json", "{\"ok\":true}");
    Serial.printf("[API] Relay %d ON\n", id);
  } else {
    server.send(400, "application/json", "{\"ok\":false,\"msg\":\"invalid id\"}");
  }
}

static void handleApiOff() {
  int id = server.arg("id").toInt();
  if (id >= 1 && id <= 12) {
    relayOff[id - 1]();
    relayState[id - 1] = RELAY_OFF;
    server.send(200, "application/json", "{\"ok\":true}");
    Serial.printf("[API] Relay %d OFF\n", id);
  } else {
    server.send(400, "application/json", "{\"ok\":false,\"msg\":\"invalid id\"}");
  }
}

static void handleApiBlink() {
  int id = server.arg("id").toInt();
  if (id >= 1 && id <= 12) {
    relayBlink[id - 1]();
    relayState[id - 1] = RELAY_BLINK;
    server.send(200, "application/json", "{\"ok\":true}");
    Serial.printf("[API] Relay %d BLINK\n", id);
  } else {
    server.send(400, "application/json", "{\"ok\":false,\"msg\":\"invalid id\"}");
  }
}

static void handleApiStatus() {
  String json = "{\"ip\":\"" + WiFi.localIP().toString() + "\",\"states\":[";
  for (int i = 0; i < 12; i++) {
    if (i > 0) json += ",";
    json += String((int)relayState[i]);
  }
  json += "]}";
  server.send(200, "application/json", json);
}

static void handleRoot() {
  server.send(200, "text/html; charset=utf-8", HTML_PAGE);
}

/* ════════════════════════════════════════════
   公开函数
   ════════════════════════════════════════════ */

void setupWebServer(void)
{
  server.on("/", handleRoot);
  server.on("/api/on", HTTP_GET, handleApiOn);
  server.on("/api/off", HTTP_GET, handleApiOff);
  server.on("/api/blink", HTTP_GET, handleApiBlink);
  server.on("/api/status", HTTP_GET, handleApiStatus);

  server.begin();
  Serial.println("Web server started on port 80");
}

void webServerLoop(void)
{
  server.handleClient();
  delay(2);
}
