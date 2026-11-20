# 2048 — Terminal Edition (C)

Saf **C11** ile yazılmış, sıfır dış bağımlılığa sahip, renkli ve akıcı bir terminal **2048** oyunu.

## Özellikler

- **Renkli Terminal Arayüzü:** Her taş değeri (`2` – `4096+`) için ayrı ANSI 256-renk paleti ve titremesiz (flicker-free) ekran yenileme.
- **Hamle Geri Alma (Undo):** Son 50 hamleye kadar geri alma desteği (`U` veya `Z`).
- **Kalıcı Yüksek Skor:** En yüksek skor otomatik olarak `.2048_highscore` dosyasına kaydedilir.
- **Sonsuz Mod:** `2048` taşına ulaştıktan sonra ister yeni oyun başlatabilir ister `4096+` hedefiyle devam edebilirsin.
- **Çapraz Platform:** Windows (`MSVC`, `MinGW/GCC`, `Clang`), Linux ve macOS desteği.
- **Birim Testleri:** Oyun mantığı (birleştirme kuralları, geri alma, kazanma/kaybetme, skor kaydı) için hazır C test paketi.

## Kontroller

| Tuş | İşlev |
| --- | --- |
| `↑` `↓` `←` `→` veya `W` `A` `S` `D` | Taşları kaydır |
| `U` veya `Z` | Son hamleyi geri al (Undo) |
| `R` | Oyunu yeniden başlat |
| `C` | 2048'e ulaştıktan sonra devam et |
| `Q` veya `Esc` | Çıkış |

## Derleme ve Çalıştırma

### Windows

`build.bat` sistemdeki `gcc`, `clang` veya Visual Studio (`MSVC`) derleyicisini otomatik algılar:

```cmd
build.bat
2048.exe
```

Birim testlerini çalıştırmak için:

```cmd
build.bat test
```

### Linux / macOS (veya MinGW)

```bash
make
./2048
```

Birim testlerini çalıştırmak için:

```bash
make test
```

## Proje Yapısı

```text
c-2048/
├── src/
│   ├── main.c       # Oyun döngüsü ve kullanıcı komutları
│   ├── game.h/c     # Tahta mantığı, kaydırma/birleştirme ve undo geçmişi
│   ├── render.h/c   # ANSI 256-renkli terminal çizimi
│   ├── input.h/c    # Windows (conio.h) ve POSIX (termios) tuş okuma
│   └── storage.h/c  # Kalıcı yüksek skor okuma/yazma
├── tests/
│   └── test_game.c  # Oyun mantığı birim testleri
├── Makefile
├── build.bat
└── .github/workflows/ci.yml
```

## Lisans

Bu proje [MIT Lisansı](LICENSE) ile lisanslanmıştır.
