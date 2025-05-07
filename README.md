Deneyap MITM Dedektörü

Bu proje, Deneyap Kart 1A V2 (ESP32-S3 tabanlı) kullanarak pasif bir Man-in-the-Middle (MITM) algılama ajanı oluşturur. Ağdaki şüpheli aktiviteleri izler, ARP spoofing ve deauth saldırılarını tespit eder, yakalanan paketleri görselleştirir ve sonuçları bir web arayüzünde sunar. Proje, siber güvenlik eğitimleri ve ağ izleme için ideal bir araçtır.

Özellikler

Wi-Fi Paket Yakalama: Promiscuous modda ARP ve deauth paketlerini yakalar.
ARP Spoofing Tespiti: Aynı IP adresine birden fazla MAC adresi eşleşmesini algılar.
Deauth Paket Tespiti: Şüpheli deauthentication çerçevelerini yakalar.
Web Arayüzü: Mevcut kanal, uyarılar ve yakalanan paketlerin tablo halinde görselleştirilmesi (otomatik yenileme ile).
Otomatik Kanal Tarama: Wi-Fi kanallarını (1-13) her 5 saniyede bir tarar.
Seri Monitör Kaydı: Tespit edilen anomalileri ve paket bilgilerini seri monitörde listeler.
Türkçe Karakter Desteği: UTF-8 ile düzgün Türkçe karakter görüntüleme.

Gereksinimler
Donanım

Deneyap Kart 1A V2 (ESP32-S3 tabanlı)
USB-C Kablosu
Bilgisayar (Windows, macOS veya Linux)

Yazılım

Deneyap Kart IDE: Resmi İndirme Sayfası
Kütüphaneler:
ESPAsyncWebServer
AsyncTCP


Test Ortamı: Wi-Fi ağı ve isteğe bağlı MITM simülasyon araçları (ör. Kali Linux ile arpspoof, aireplay-ng)

Kurulum
1. Deneyap Kart IDE’yi Kurma

Deneyap Kart IDE’yi indirin ve kurun.
IDE’yi açın ve Araçlar > Kart menüsünden Deneyap Kart 1A V2’yi seçin.
Portu seçin: Araçlar > Port menüsünden kartın bağlı olduğu COM portunu belirleyin.

2. Kütüphaneleri Yükleme

IDE’de Araçlar > Kütüphane Yöneticisi’ni açın.
ESPAsyncWebServer ve AsyncTCP kütüphanelerini aratın ve kurun.
Alternatif olarak, kütüphaneleri GitHub’dan indirip manuel ekleyin:
ESPAsyncWebServer
AsyncTCP



3. Kodu Yükleme

Bu depodaki MITM_Detector.ino dosyasını indirin.
Deneyap Kart IDE’yi açın ve kodu yapıştırın veya dosyayı açın.
Deneyap Kart 1A V2’yi USB-C kablosuyla bilgisayara bağlayın.
Derle (✔) ve Yükle (→) butonlarını kullanarak kodu karta yükleyin.

Kullanım
1. Seri Monitörü Açma

IDE’de Araçlar > Seri Monitör’ü açın (baud rate: 115200).
Kartın IP adresini (ör. 192.168.4.1), yakalanan paketleri ve tespit edilen anomalileri görün.
Örnek çıktı:AP Başlatıldı. IP: 192.168.4.1
Yeni IP-MAC Eşleşmesi: 192.168.1.100 -> aa:bb:cc:dd:ee:ff
ARP Spoofing Tespit Edildi! IP: 192.168.1.100, Eski MAC: aa:bb:cc:dd:ee:ff, Yeni MAC: 11:22:33:44:55:66



2. Web Arayüzüne Bağlanma

Telefonunuz veya bilgisayarınızda Wi-Fi ağlarını tarayın ve Deneyap_MITM_Detector ağına bağlanın.
Tarayıcınızda kartın IP adresini açın (ör. http://192.168.4.1).
Web arayüzünde şunları göreceksiniz:
Mevcut kanal (ör. “Mevcut Kanal: 3”).
Uyarılar (ör. “ARP Spoofing Tespit Edildi!”).
Yakalanan paketlerin tablosu (Tür, Kaynak IP, Kaynak MAC, Kanal, Zaman).


Sayfa her 5 saniyede bir otomatik yenilenir.

3. Test Etme

Test Ortamı Kurma:
Kendi Wi-Fi ağınızda veya bir test ortamında sahte MITM saldırıları simüle edin.
Önerilen araçlar:
Kali Linux ile arpspoof: arpspoof -i wlan0 -t 192.168.1.100 192.168.1.1
Aircrack-ng ile aireplay-ng: aireplay-ng --deauth 10 -a <AP_MAC> wlan0




Sonuçları Gözlemleme:
Seri monitörde ve web arayüzünde ARP spoofing veya deauth paketleri için uyarılar görünecek.
Yakalanan paketler, web arayüzünde bir tablo olarak listelenecek.



Web Arayüzü Örneği
Deneyap MITM Dedektörü
Mevcut Kanal: 3
Uyarılar:
ARP Spoofing Tespit Edildi! IP: 192.168.1.100, Eski MAC: aa:bb:cc:dd:ee:ff, Yeni MAC: 11:22:33:44:55:66

Yakalanan Paketler
Tür     | Kaynak IP        | Kaynak MAC           | Kanal | Zaman
ARP     | 192.168.1.100    | aa:bb:cc:dd:ee:ff   | 3     | 2dk 15sn
Deauth  | -                | -                   | 3     | 2dk 20sn

Sınırlamalar

İşlem Gücü: ESP32-S3, derinlemesine paket analizi için sınırlıdır. Karmaşık analizler için Raspberry Pi düşünülebilir.
Şifreli Trafik: HTTPS gibi şifreli trafik analiz edilemez; yalnızca meta veriler (IP, MAC, çerçeve tipi) incelenir.
Yasal Uyarı: Bu projeyi yalnızca kendi ağınızda veya izinli test ortamlarında kullanın.
Hafıza: En fazla 50 paket saklanır (MAX_PACKETS). Daha fazla saklama için RAM kullanımına dikkat edin.

Özelleştirmeler

Paket Sayısı: MAX_PACKETS değerini değiştirerek saklanan paket sayısını ayarlayabilirsiniz.
Yenileme Süresi: Web yenileme süresini değiştirmek için <meta http-equiv='refresh' content='5'> içindeki 5 değerini güncelleyin.
Filtreleme: Belirli paket türlerini göstermek için web arayüzüne filtre eklenebilir.

Katkıda Bulunma
Katkılarınızı bekliyoruz! Lütfen aşağıdaki adımları izleyin:

Bu depoyu fork edin.
Yeni özellikler veya düzeltmeler için bir dal oluşturun (git checkout -b feature/yeni-ozellik).
Değişikliklerinizi commit edin (git commit -m 'Yeni özellik eklendi').
Dalınızı push edin (git push origin feature/yeni-ozellik).
Bir Pull Request açın.


