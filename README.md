# OpenCAD Pro

**OpenCAD Pro**, sohbet ekindeki kapsamlı CAD fikrinden yola çıkan erken aşamadaki açık kaynaklı bir geometri prototipidir. Bu depo henüz SolidWorks/NX/CATIA/AutoCAD eşdeğeri değildir; aşağıdaki çalışan temel, büyük CAD mimarisine başlanabilmesi için derlenebilir bir MVP olarak sunulur.

## Şu an çalışanlar

- C++20 geometri kütüphanesi
- Basit, kendi kendini kesmeyen 2B poligonları üçgenleştirme
- 2B profilin +Z yönünde parametrik ekstrüzyonu
- Wavefront OBJ 3B mesh ve SVG 2B eskiz dışa aktarımı
- Dikdörtgen katı oluşturmak için komut satırı arayüzü
- Harici test kütüphanesi gerektirmeyen otomatik testler
- Linux, macOS ve Windows için GitHub Actions derleme/test matrisi

## Hızlı başlangıç

Gereksinimler: C++20 derleyicisi ve CMake 3.16 veya üstü. Prototipin temelini derlemek için Qt/OpenCASCADE gibi CAD bağımlılıkları henüz gerekmez.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Bir örnek OBJ katı ve SVG eskiz üretin:

```bash
./build/opencadpro --demo demo.obj
```

Boyutları kendiniz belirleyin (birim: mm):

```bash
./build/opencadpro --box 120 80 12 --output plate.obj --svg plate.svg
```

Windows'ta çalıştırılabilir dosya `build/Release/opencadpro.exe` altında olabilir.

## Kapsam ve sınırlamalar

Bu MVP'de GUI, parametrik özellik ağacı, gerçek B-Rep/OCCT geometrisi, STEP/DXF/DWG, montaj, FEM, sac metal, standart profil kütüphanesi ve Python eklenti API'si **henüz uygulanmamıştır**. OBJ dışa aktarımı üçgen mesh üretir; mühendislik üretiminde kullanılmadan önce geometri/ölçü doğrulaması gerekir. Şimdilik yalnızca basit, kendi kendini kesmeyen poligonlar desteklenir.

Sohbet kaydı, kod deposu değil, özellik planı ve kısmi kod örnekleri içerdiğinden bu depoda sohbet boyunca var olduğu söylenen binlerce testi veya tamamlanmış modülleri iddia etmiyoruz. İleri geliştirme aşamaları `docs/ROADMAP.md` dosyasındadır.

## Sürüm notu

İlk depo commit'indeki `v2.2.0` etiketi için bu depoda kaynak kodu veya release kaydı bulunmadığından, mevcut derlenebilir prototipin sürümü `0.1.0` olarak belirlenmiştir. İlk README içeriği ve bu karar `docs/REPOSITORY_HISTORY.md` dosyasında korunmuştur.

## Lisans

LGPL-2.1-or-later. Ayrıntılar için `LICENSE` dosyasına bakın.
