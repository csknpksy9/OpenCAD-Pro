# Yol Haritası

Bu plan bir taahhüt veya mevcut özellik beyanı değil, sohbet ekindeki hedeflerin gerçekçi geliştirme sırasına çevrilmiş taslağıdır.

## 0.1 — Derlenebilir geometri prototipi (bu depo)

- [x] C++20 temel geometri kütüphanesi
- [x] 2B poligon ekstrüzyonu
- [x] SVG ve OBJ dışa aktarımı
- [x] CMake, testler ve CI

## 0.2 — Çekirdek iş akışları

- Eskiz veri modeli, katmanlar ve boyutlandırma
- Proje dosyası biçimi, undo/redo ve parametre doğrulama
- Daha kapsamlı poligon onarımı ve mesh doğrulaması

## 0.3 — CAD çekirdeği/arayüz seçimi

- OpenCASCADE Technology (OCCT) ve FreeCAD eklenti/fork seçeneklerinin teknik ve lisans incelemesi
- Qt tabanlı GUI ve viewport prototipi
- STEP/STL içe aktarma/dışa aktarma için bağımlılık yönetimi

## 0.4+ — Uzmanlaşmış modüller

- Parametrik özellik ağacı ve sketch solver
- Sac metal ve profil kütüphanesi
- Montaj, teknik resim ve BOM
- FEM ağı/çözücü adaptörleri ve sonuç görselleştirmesi
- Python API, makrolar ve eklenti sistemi

Her aşamada özellikler yalnızca kod, test ve kullanıcı dokümantasyonu gerçekten eklendikten sonra tamamlandı olarak işaretlenmelidir.
