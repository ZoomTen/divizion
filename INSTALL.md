as native
```bash
meson setup build \
  -Dstandalone=true \
  -Dvst2=false
```

as vst2

```bash
meson setup build \
  --cross-file cross-i686-mingw32.ini \
  --prefix=/usr/i686-w64-mingw32 \
  -Dstandalone=false \
  -Dvst2=true \
  --buildtype=release
```

```bash
cd build
ninja
```
If build fails just try running `ninja` a few more times nbd