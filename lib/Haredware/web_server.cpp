#include "web_server.h"
#include "relay.h"
#include "sensor.h"
#include <WiFi.h>

WebServer server(80);

/* ── 继电器状态 ── */
enum RelayState { RELAY_OFF, RELAY_ON, RELAY_BLINK };
static RelayState relayState[12] = { RELAY_OFF };

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
   HTML — 仪表盘 + 继电器双标签页
   ════════════════════════════════════════════ */
static const char HTML_PAGE[] = R"====(<!DOCTYPE html>
<html lang="zh-CN">
<head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>环境监测控制</title>
<script src="https://cdn.tailwindcss.com"></script>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<style>
.badge-ok{background:#dcfce7;color:#15803d;border:1px solid #bbf7d0;border-radius:9999px}
.badge-warn{background:#fef9c3;color:#a16207;border:1px solid #fef08a;border-radius:9999px}
.badge-err{background:#fee2e2;color:#b91c1c;border:1px solid #fecaca;border-radius:9999px}
.badge-off{background:#f3f4f6;color:#6b7280;border:1px solid #e5e7eb;border-radius:9999px}
.led-on{box-shadow:0 0 10px currentColor;animation:ledBlink 1.6s ease-in-out infinite alternate}
@keyframes ledBlink{from{opacity:.7}to{opacity:1}}
</style></head>
<body class="bg-slate-100 text-gray-800 min-h-screen">
<header class="bg-emerald-700 text-white shadow-md sticky top-0 z-40">
<div class="max-w-full mx-auto px-4 py-3 flex items-center justify-between">
<div class="flex items-center space-x-3">
<div class="w-9 h-9 rounded-full bg-emerald-500 flex items-center justify-center"><i class="fas fa-microchip text-xl"></i></div>
<div><div class="text-lg font-semibold">环境监测控制系统</div><div class="text-xs text-emerald-100">Environmental Monitoring</div></div></div>
<div class="flex items-center space-x-4 text-sm">
<span id="hdr-ip" class="text-xs text-emerald-100 font-mono">IP: --</span>
<span id="hdr-time" class="font-mono">--:--:--</span>
<span id="hdr-status"><span class="w-2 h-2 rounded-full bg-green-400 inline-block mr-1"></span>在线</span></div></div></header>
<main class="max-w-full mx-auto px-4 py-4 space-y-4">
<div class="bg-white rounded-2xl shadow-sm p-4 flex items-center justify-between flex-wrap gap-3">
<div class="flex items-center space-x-3"><h1 class="text-xl font-bold text-emerald-700">实时数据</h1><span id="overall-tag" class="text-xs px-2.5 py-1 badge-off">加载中</span></div>
<div><button onclick="sw(0)" class="px-4 py-2 rounded-xl bg-emerald-600 text-white text-sm font-semibold mr-2" id="btn-dash">仪表盘</button>
<button onclick="sw(1)" class="px-4 py-2 rounded-xl border border-gray-300 text-gray-600 text-sm font-semibold" id="btn-relay">继电器</button></div></div>
<div id="tab-dash">
<div class="grid grid-cols-1 lg:grid-cols-3 gap-4 mb-4">
<div class="lg:col-span-2 space-y-4"><div class="grid grid-cols-2 sm:grid-cols-4 gap-3" id="scards"></div>
<div class="grid grid-cols-1 md:grid-cols-2 gap-4">
<div class="bg-white rounded-2xl shadow-sm p-4"><div class="flex items-center space-x-2 mb-3"><div class="w-8 h-8 rounded-full bg-sky-100 flex items-center justify-center"><i class="fas fa-wind text-sky-500"></i></div><span class="font-semibold">风速 & 风向</span></div>
<div class="flex items-baseline space-x-1 mb-2"><span id="ws-val" class="text-2xl font-bold">--</span><span class="text-sm text-gray-500">m/s</span></div><div class="w-full h-1.5 bg-gray-100 rounded-full overflow-hidden mb-3"><div id="ws-bar" class="h-full rounded-full bg-sky-400" style="width:0%"></div></div>
<div class="flex items-center space-x-4"><div class="relative w-20 h-20 rounded-full border border-gray-300 flex items-center justify-center text-xs text-gray-400"><span class="absolute top-0.5 left-1/2 -translate-x-1/2">N</span><span class="absolute bottom-0.5 left-1/2 -translate-x-1/2">S</span><span class="absolute right-1 top-1/2 -translate-y-1/2">E</span><span class="absolute left-1 top-1/2 -translate-y-1/2">W</span><div id="wd-arrow" class="absolute left-1/2 top-1/2 w-0.5 h-7 bg-orange-400 rounded-full" style="transform:translate(-50%,-100%) rotate(0deg);transform-origin:bottom center"></div></div>
<div><div class="flex items-baseline space-x-1"><span id="wd-val" class="text-xl font-bold">--</span><span class="text-xs text-gray-500">°</span></div><div id="wd-text" class="text-xs text-gray-500 mt-1">--</div></div></div></div>
<div class="bg-white rounded-2xl shadow-sm p-4"><div class="flex items-center space-x-2 mb-3"><div class="w-8 h-8 rounded-full bg-emerald-100 flex items-center justify-center"><i class="fas fa-heartbeat text-emerald-600"></i></div><span class="font-semibold">综合评分</span></div>
<div class="flex items-baseline space-x-2"><span id="score-val" class="text-3xl font-bold">--</span><span class="text-sm text-gray-500">/ 100</span></div><div id="score-text" class="mt-1 text-xs text-gray-600">--</div><div class="mt-2 h-1.5 bg-gray-100 rounded-full overflow-hidden"><div id="score-bar" class="h-full rounded-full bg-emerald-500" style="width:0%"></div></div></div></div></div>
<div class="bg-white rounded-2xl shadow-sm p-4"><div class="flex items-center space-x-2 mb-3"><div class="w-8 h-8 rounded-full bg-amber-100 flex items-center justify-center"><i class="fas fa-seedling text-amber-600"></i></div><span class="font-semibold">土壤环境</span></div>
<div class="grid grid-cols-2 gap-3"><div class="border rounded-xl p-3"><div class="text-xs text-gray-500">土壤湿度</div><div class="flex items-baseline space-x-1"><span id="sm-val" class="text-xl font-bold">--</span><span class="text-xs text-gray-500">%</span></div><div class="w-full h-1.5 bg-gray-100 rounded-full overflow-hidden mt-2"><div id="sm-bar" class="h-full rounded-full bg-emerald-400" style="width:0%"></div></div></div>
<div class="border rounded-xl p-3"><div class="text-xs text-gray-500">土壤温度</div><div class="flex items-baseline space-x-1"><span id="st-val" class="text-xl font-bold">--</span><span class="text-xs text-gray-500">°C</span></div><div class="w-full h-1.5 bg-gray-100 rounded-full overflow-hidden mt-2"><div id="st-bar" class="h-full rounded-full bg-orange-400" style="width:0%"></div></div></div>
<div class="border rounded-xl p-3"><div class="text-xs text-gray-500">电导率 EC</div><div class="flex items-baseline space-x-1"><span id="sec-val" class="text-xl font-bold">--</span><span class="text-xs text-gray-500">μS/cm</span></div><div class="w-full h-1.5 bg-gray-100 rounded-full overflow-hidden mt-2"><div id="sec-bar" class="h-full rounded-full bg-purple-400" style="width:0%"></div></div></div>
<div class="border rounded-xl p-3"><div class="text-xs text-gray-500">氧气 O₂</div><div class="flex items-baseline space-x-1"><span id="o2-val" class="text-xl font-bold">--</span><span class="text-xs text-gray-500">%</span></div><div class="w-full h-1.5 bg-gray-100 rounded-full overflow-hidden mt-2"><div id="o2-bar" class="h-full rounded-full bg-cyan-400" style="width:0%"></div></div></div></div></div>
<div class="bg-white rounded-2xl shadow-sm p-4"><div class="flex items-center space-x-2 mb-3"><i class="fas fa-chart-line text-emerald-600"></i><span class="font-semibold">趋势图</span></div>
<div class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-3" id="charts"></div></div></div>
<div id="tab-relay" class="hidden"><div class="grid grid-cols-2 sm:grid-cols-3 md:grid-cols-4 lg:grid-cols-6 gap-3" id="rgrid"></div></div></main>
<div id="toast" class="fixed top-4 right-4 px-4 py-2 rounded-lg text-white text-sm z-50 opacity-0 transition-opacity pointer-events-none"></div>
<script>
function sw(n){document.getElementById('tab-dash').className=n===0?'':'hidden';document.getElementById('tab-relay').className=n===1?'':'hidden';document.getElementById('btn-dash').className=n===0?'px-4 py-2 rounded-xl bg-emerald-600 text-white text-sm font-semibold mr-2':'px-4 py-2 rounded-xl border border-gray-300 text-gray-600 text-sm font-semibold mr-2';document.getElementById('btn-relay').className=n===1?'px-4 py-2 rounded-xl bg-emerald-600 text-white text-sm font-semibold':'px-4 py-2 rounded-xl border border-gray-300 text-gray-600 text-sm font-semibold'}
var sc=document.getElementById('scards');
[{id:'ws',ic:'💨',n:'风速',u:'m/s',c:'#06b6d4',mn:0,mx:20},{id:'at',ic:'🌡',n:'气温',u:'°C',c:'#f97316',mn:-10,mx:50},{id:'ah',ic:'💧',n:'湿度',u:'%RH',c:'#3b82f6',mn:0,mx:100},{id:'lux',ic:'☀',n:'光照',u:'lux',c:'#f59e0b',mn:0,mx:5000},{id:'co2',ic:'🌫',n:'CO2',u:'ppm',c:'#64748b',mn:300,mx:2000},{id:'sm',ic:'🌱',n:'土湿',u:'%',c:'#22c55e',mn:0,mx:100},{id:'st',ic:'🌡',n:'土温',u:'°C',c:'#a855f7',mn:-10,mx:50},{id:'o2',ic:'🫧',n:'氧气',u:'%',c:'#06b6d4',mn:0,mx:30}].forEach(function(s){
var d=document.createElement('div');d.className='bg-white rounded-xl shadow-sm p-3 text-center border';d.id='sc-'+s.id;
d.innerHTML='<div class="text-lg">'+s.ic+'</div><div class="text-lg font-bold mt-1" style="color:'+s.c+'">--</div><div class="text-xs text-gray-400">'+s.n+' <span class="text-gray-300">'+s.u+'</span></div>';
sc.appendChild(d)});
var rdf=[{id:'ws',lb:'风速 (m/s)',co:'#06b6d4'},{id:'at',lb:'气温 (°C)',co:'#f97316'},{id:'ah',lb:'湿度 (%RH)',co:'#3b82f6'},{id:'lux',lb:'光照 (lux)',co:'#f59e0b'},{id:'co2',lb:'CO2 (ppm)',co:'#64748b'},{id:'sm',lb:'土壤湿度 (%)',co:'#22c55e'}];
var cls={},clen=30;
var cw=document.getElementById('charts');
rdf.forEach(function(c){var b=document.createElement('div');b.className='bg-white rounded-xl shadow-sm p-3 border';b.innerHTML='<div class="text-xs text-gray-500 mb-1">'+c.lb+'</div><div class="h-32"><canvas id="ch-'+c.id+'"></canvas></div>';cw.appendChild(b)});
rdf.forEach(function(c){var cv=document.getElementById('ch-'+c.id);cv.getContext('2d');var ch=new Chart(cv,{type:'line',data:{labels:[],datasets:[{label:c.lb,borderColor:c.co,backgroundColor:c.co+'22',tension:.3,fill:true,data:[]}]},options:{responsive:true,maintainAspectRatio:false,scales:{x:{display:false},y:{beginAtZero:true}},plugins:{legend:{display:false}}}});cls[c.id]=ch});
setInterval(function(){var n=new Date();document.getElementById('hdr-time').textContent=n.toLocaleTimeString('zh-CN',{hour12:false})},1000);
var rg=document.getElementById('rgrid');
for(var i=1;i<=12;i++){(function(id){var c=document.createElement('div');c.className='bg-white rounded-xl shadow-sm p-3 text-center border';
c.innerHTML='<div class="text-xs text-gray-400 mb-1">通道 '+id+'</div><div class="w-3 h-3 rounded-full mx-auto mb-1 bg-gray-300" id="rd'+id+'"></div><div class="text-sm font-semibold mb-2" id="rl'+id+'">OFF</div><div class="flex gap-1 justify-center"><button class="px-2 py-1 rounded text-xs text-white font-semibold bg-green-500" data-cmd="on" data-id="'+id+'">ON</button><button class="px-2 py-1 rounded text-xs text-white font-semibold bg-red-500" data-cmd="off" data-id="'+id+'">OFF</button></div>';
rg.appendChild(c);c.querySelectorAll('button').forEach(function(btn){btn.onclick=function(){rCmd(id,btn.getAttribute('data-cmd'))}})})(i)}
function rCmd(id,cmd){fetch('/api/'+cmd+'?id='+id).then(function(r){return r.json()}).then(function(d){if(d.ok){updateR(id,cmd);ts('通道 '+id+': '+cmd.toUpperCase(),'bg-green-500')}})}
function updateR(id,cmd){var d=document.getElementById('rd'+id),l=document.getElementById('rl'+id);d.className='w-3 h-3 rounded-full mx-auto mb-1 '+(cmd==='on'?'bg-green-500':'bg-gray-300');l.textContent=cmd==='on'?'ON':'OFF';l.style.color=cmd==='on'?'#22c55e':'#6b7280'}
function ts(m,c){var e=document.getElementById('toast');e.textContent=m;e.className='fixed top-4 right-4 px-4 py-2 rounded-lg text-white text-sm z-50 opacity-100 transition-opacity pointer-events-none '+c;clearTimeout(ts._t);ts._t=setTimeout(function(){e.className='fixed top-4 right-4 px-4 py-2 rounded-lg text-white text-sm z-50 opacity-0 transition-opacity pointer-events-none'},1500)}
function uc(d){var el=document.getElementById('wd-arrow');if(el)el.style.transform='translate(-50%,-100%) rotate('+d+'deg)'}
function setBar(el,v,mn,mx){var p=Math.max(0,Math.min(100,(v-mn)/(mx-mn)*100));el.style.width=p+'%'}
function sv(id,v){var el=document.getElementById('sc-'+id);if(el)el.querySelector('.font-bold').textContent=v}
function refresh(){fetch('/api/sensors').then(function(r){return r.json()}).then(function(d){
document.getElementById('hdr-ip').textContent='IP: '+d.ip;
sv('ws',d.ws.toFixed(1));sv('at',d.at.toFixed(1));sv('ah',d.ah.toFixed(1));sv('lux',Math.round(d.lux));sv('co2',Math.round(d.co2));sv('sm',d.sm.toFixed(1));sv('st',d.st.toFixed(1));sv('o2',d.o2.toFixed(1));
document.getElementById('ws-val').textContent=d.ws.toFixed(1);setBar(document.getElementById('ws-bar'),d.ws,0,20);
document.getElementById('wd-val').textContent=d.wd.toFixed(1);document.getElementById('wd-text').textContent='风向: '+d.wdn;uc(d.wd);
document.getElementById('sm-val').textContent=d.sm.toFixed(1);setBar(document.getElementById('sm-bar'),d.sm,0,100);
document.getElementById('st-val').textContent=d.st.toFixed(1);setBar(document.getElementById('st-bar'),d.st,-10,50);
document.getElementById('sec-val').textContent=Math.round(d.sec);setBar(document.getElementById('sec-bar'),d.sec,0,5000);
document.getElementById('o2-val').textContent=d.o2.toFixed(1);setBar(document.getElementById('o2-bar'),d.o2,0,30);
var sc=Math.min(100,Math.max(0,100-(d.ws>15?15:0)-(d.at>35||d.at<5?20:0)-(d.ah>85||d.ah<20?15:0)-(d.sm<20||d.sm>85?20:0)));
document.getElementById('score-val').textContent=Math.round(sc);setBar(document.getElementById('score-bar'),sc,0,100);
document.getElementById('score-bar').style.backgroundColor=sc>=80?'#22c55e':sc>=60?'#facc15':'#f97373';
document.getElementById('score-text').textContent=sc>=85?'环境良好':sc>=60?'基本适宜':'需关注';
var tag=document.getElementById('overall-tag');tag.textContent=sc>=80?'环境正常':sc>=60?'轻微异常':'需关注';tag.className='text-xs px-2.5 py-1 '+(sc>=80?'badge-ok':sc>=60?'badge-warn':'badge-err');
var f={ws:d.ws,at:d.at,ah:d.ah,lux:d.lux,co2:d.co2,sm:d.sm};
Object.keys(f).forEach(function(k){var ch=cls[k];if(!ch)return;ch.data.labels.push('');ch.data.datasets[0].data.push(f[k]);if(ch.data.labels.length>clen){ch.data.labels.shift();ch.data.datasets[0].data.shift()}ch.update()});
for(var i=1;i<=12;i++)updateR(i,d.rly[i-1]===1?'on':'off')
})}setInterval(refresh,2000);refresh();
</script></body></html>)====";

/* ════════════════════════════════════════════
   API — /api/sensors (JSON)
   ════════════════════════════════════════════ */

static void handleApiSensors() {
  float ws = readWindSpeed();
  WindDirData wd; bool wdOk = readWindDirData(&wd);
  float at = readAirTemp(), ah = readAirHumidity(), lux = readLight();
  SoilData soil; bool soilOk = readSoilData(&soil);
  int co2 = readCO2(); float o2 = readOxygen();

  char buf[512];
  snprintf(buf, sizeof(buf),
    "{\"ip\":\"%s\","
    "\"ws\":%.1f,\"wd\":%.1f,\"wdn\":\"%s\","
    "\"at\":%.1f,\"ah\":%.1f,\"lux\":%.0f,"
    "\"sm\":%.1f,\"st\":%.1f,\"sec\":%.0f,"
    "\"co2\":%d,\"o2\":%.1f,"
    "\"rly\":[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]}",
    WiFi.localIP().toString().c_str(),
    ws, wdOk ? wd.angle : -1.0f, wdOk ? wd.name : "?",
    at, ah, lux,
    soilOk ? soil.moisture : -1.0f, soilOk ? soil.temp : -999.0f, soilOk ? soil.ec : -1.0f,
    co2, o2,
    (int)relayState[0],(int)relayState[1],(int)relayState[2],(int)relayState[3],
    (int)relayState[4],(int)relayState[5],(int)relayState[6],(int)relayState[7],
    (int)relayState[8],(int)relayState[9],(int)relayState[10],(int)relayState[11]
  );
  server.send(200, "application/json", buf);
}

/* ── 继电器 API ── */
static void handleApiOn() {
  int id = server.arg("id").toInt();
  if (id >= 1 && id <= 12) {
    relayOn[id - 1](); relayState[id - 1] = RELAY_ON;
    server.send(200, "application/json", "{\"ok\":true}");
  } else { server.send(400, "application/json", "{\"ok\":false}"); }
}
static void handleApiOff() {
  int id = server.arg("id").toInt();
  if (id >= 1 && id <= 12) {
    relayOff[id - 1](); relayState[id - 1] = RELAY_OFF;
    server.send(200, "application/json", "{\"ok\":true}");
  } else { server.send(400, "application/json", "{\"ok\":false}"); }
}
static void handleApiBlink() {
  int id = server.arg("id").toInt();
  if (id >= 1 && id <= 12) {
    relayBlink[id - 1](); relayState[id - 1] = RELAY_BLINK;
    server.send(200, "application/json", "{\"ok\":true}");
  } else { server.send(400, "application/json", "{\"ok\":false}"); }
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
static void handleRoot() { server.send(200, "text/html; charset=utf-8", HTML_PAGE); }

/* ════════════════════════════════════════════
   公开函数
   ════════════════════════════════════════════ */
void setupWebServer(void) {
  server.on("/", handleRoot);
  server.on("/api/sensors", HTTP_GET, handleApiSensors);
  server.on("/api/on", HTTP_GET, handleApiOn);
  server.on("/api/off", HTTP_GET, handleApiOff);
  server.on("/api/blink", HTTP_GET, handleApiBlink);
  server.on("/api/status", HTTP_GET, handleApiStatus);
  server.begin();
  Serial.println("Web server started on port 80");
}
void webServerLoop(void) { server.handleClient(); delay(2); }
