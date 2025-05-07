#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "esp_wifi.h"
#include <map>
#include <string>
#include <vector>

// Web sunucusu için AsyncWebServer nesnesi
AsyncWebServer server(80);

// IP-MAC eşleşmelerini saklamak için map
std::map<String, String> ipMacMap;

// Şüpheli aktiviteleri saklamak için string
String alerts = "";

// Yakalanan paketleri saklamak için yapı ve vektör
struct PacketInfo {
  String type;      // Paket tipi (ARP, Deauth)
  String srcIP;     // Kaynak IP (ARP için)
  String srcMAC;    // Kaynak MAC (ARP için)
  int channel;      // Kanal
  String timestamp; // Zaman damgası
};
std::vector<PacketInfo> capturedPackets;
const int MAX_PACKETS = 50; // Maksimum saklanacak paket sayısı

// Mevcut Wi-Fi kanalı
int currentChannel = 1;

// Zaman damgası oluşturma
String getTimestamp() {
  unsigned long ms = millis();
  unsigned long seconds = ms / 1000;
  unsigned long minutes = seconds / 60;
  seconds %= 60;
  return String(minutes) + "dk " + String(seconds) + "sn";
}

// Paket sniffer callback fonksiyonu
void sniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  uint8_t* payload = pkt->payload;
  PacketInfo packet;

  // ARP paketi kontrolü (EtherType 0x0806)
  if (payload[12] == 0x08 && payload[13] == 0x06) {
    // ARP paketi: Kaynak IP ve MAC adresini al
    String srcIP = String(payload[28]) + "." + String(payload[29]) + "." + 
                   String(payload[30]) + "." + String(payload[31]);
    String srcMAC = String(payload[22], HEX) + ":" + String(payload[23], HEX) + ":" +
                    String(payload[24], HEX) + ":" + String(payload[25], HEX) + ":" +
                    String(payload[26], HEX) + ":" + String(payload[27], HEX);

    // Paket bilgilerini kaydet
    packet.type = "ARP";
    packet.srcIP = srcIP;
    packet.srcMAC = srcMAC;
    packet.channel = currentChannel;
    packet.timestamp = getTimestamp();
    capturedPackets.push_back(packet);
    if (capturedPackets.size() > MAX_PACKETS) {
      capturedPackets.erase(capturedPackets.begin());
    }

    // IP-MAC eşleşmesini kontrol et
    if (ipMacMap.find(srcIP) != ipMacMap.end()) {
      if (ipMacMap[srcIP] != srcMAC) {
        String alert = "ARP Spoofing Tespit Edildi! IP: " + srcIP + 
                       ", Eski MAC: " + ipMacMap[srcIP] + ", Yeni MAC: " + srcMAC;
        Serial.println(alert);
        alerts += alert + "<br>";
      }
    } else {
      ipMacMap[srcIP] = srcMAC;
      Serial.println("Yeni IP-MAC Eşleşmesi: " + srcIP + " -> " + srcMAC);
    }
  }

  // Deauth paketi kontrolü (802.11 yönetim çerçevesi, tip 0x0C)
  if (type == WIFI_PKT_MGMT && payload[0] == 0xC0) {
    // Deauth paketi: Bilgileri kaydet
    packet.type = "Deauth";
    packet.srcIP = "-";
    packet.srcMAC = "-";
    packet.channel = currentChannel;
    packet.timestamp = getTimestamp();
    capturedPackets.push_back(packet);
    if (capturedPackets.size() > MAX_PACKETS) {
      capturedPackets.erase(capturedPackets.begin());
    }

    String alert = "Deauth Paketi Tespit Edildi! Kanal: " + String(currentChannel);
    Serial.println(alert);
    alerts += alert + "<br>";
  }
}

// Kanal değiştirme fonksiyonu
void switchChannel() {
  currentChannel = (currentChannel % 13) + 1; // 1-13 kanalları arasında döngü
  esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
  Serial.println("Kanal Değiştirildi: " + String(currentChannel));
}

void setup() {
  // Seri monitörü başlat
  Serial.begin(115200);

  // WiFi modunu ayarla (Station modu)
  WiFi.mode(WIFI_MODE_STA);

  // Promiscuous modunu etkinleştir
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);

  // İlk kanalı ayarla
  esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);

  // Access Point oluştur (web arayüzü için)
  WiFi.softAP("Deneyap_MITM_Detector", NULL);
  Serial.println("AP Başlatıldı. IP: " + WiFi.softAPIP().toString());

  // Web sunucusu rotaları
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String html = "<html><head><title>MITM Dedektörü</title>";
    html += "<meta charset='UTF-8'>";
    html += "<meta http-equiv='refresh' content='5'>"; // 5 saniyede bir yenile
    html += "<style>";
    html += "body { font-family: Arial; margin: 20px; }";
    html += "h1 { color: #333; }";
    html += ".alert { color: red; }";
    html += "table { border-collapse: collapse; width: 100%; margin-top: 20px; }";
    html += "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }";
    html += "th { background-color: #f2f2f2; }";
    html += "</style>";
    html += "</head><body><h1>Deneyap MITM Dedektörü</h1>";
    html += "<p><b>Mevcut Kanal:</b> " + String(currentChannel) + "</p>";
    html += "<p><b>Uyarılar:</b><br>" + (alerts.length() > 0 ? alerts : "Şüpheli aktivite yok.") + "</p>";
    
    // Yakalanan paketler tablosu
    html += "<h2>Yakalanan Paketler</h2>";
    html += "<table>";
    html += "<tr><th>Tür</th><th>Kaynak IP</th><th>Kaynak MAC</th><th>Kanal</th><th>Zaman</th></tr>";
    for (const auto& packet : capturedPackets) {
      html += "<tr>";
      html += "<td>" + packet.type + "</td>";
      html += "<td>" + packet.srcIP + "</td>";
      html += "<td>" + packet.srcMAC + "</td>";
      html += "<td>" + String(packet.channel) + "</td>";
      html += "<td>" + packet.timestamp + "</td>";
      html += "</tr>";
    }
    html += "</table>";
    html += "</body></html>";
    request->send(200, "text/html; charset=UTF-8", html);
  });

  // Web sunucusunu başlat
  server.begin();
}

void loop() {
  // Kanal değiştirme zamanlayıcısını çalıştır
  static unsigned long lastSwitch = 0;
  const unsigned long switchInterval = 5000; // 5 saniye
  if (millis() - lastSwitch >= switchInterval) {
    switchChannel();
    lastSwitch = millis();
  }
}