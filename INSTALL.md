```bash
meson setup build \
  --cross-file cross-i686-mingw32.ini \
  --prefix=/usr/i686-w64-mingw32 \
  --buildtype=release
```

```bash
cd build
ninja
```
If build fails just try running `ninja` a few more times nbd