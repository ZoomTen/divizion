```bash
meson setup build \
  --cross-file cross-i686-mingw32.ini \
  --prefix=/usr/i686-w64-mingw32
```

```bash
meson compile -C build
```