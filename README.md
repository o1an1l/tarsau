# TARSAU

Sistem Programlama 2025-2026 Bahar Dönemi Projesi

## Proje Amacı

Bu projede tar/zip benzeri ancak sıkıştırma yapmayan bir arşivleme programı geliştirilmiştir.

Program Linux ortamında C dili kullanılarak yazılmıştır.

## Özellikler

- Metin dosyalarını `.sau` arşiv dosyasında birleştirir
- Arşiv dosyasını tekrar açabilir
- Dosya izinlerini korur
- Makefile ile derlenebilir
- Linux/WSL ortamında çalışır

## Kullanılan Teknolojiler

- C Programlama Dili
- GCC
- Makefile
- Ubuntu WSL

## Kullanılan Fonksiyonlar

- fopen
- fclose
- fread
- fwrite
- fgetc
- fputc
- stat
- chmod
- mkdir

## Kullanım

### Derleme

```bash
make
```

### Temizleme

```bash
make clean
```

### Arşiv oluşturma

```bash
./tarsau -b t1.txt t2.txt t3.txt -o s1.sau
```

### Arşiv açma

```bash
./tarsau -a s1.sau d1
```

## Hata Kontrolleri

Program aşağıdaki durumlarda hata mesajı vermektedir:

- Geçersiz parametre kullanımı
- Uygunsuz veya bozuk `.sau` dosyası
- Var olmayan giriş dosyası
- 32’den fazla giriş dosyası
- 200 MB’dan büyük toplam giriş boyutu

## Proje Dosyaları

- main.c
- Makefile
- README.md

## Geliştiriciler

Tunahan Avşar
Anıl Gezertaşar
