# Najważniejsze komendy

## Clean

```bash
- clean 
rm -rf build
- clean additional files, works if /build exists
cmake --build --preset release --target clean

- Release
cmake --preset release
cmake --build --preset release
./build/release/app.exe

- Release with no march=native
cmake --preset release-portable
cmake --build --preset release-portable
./build/release-portable/app.exe

- for crashhandler
cmake --preset crash-debug
cmake --build --preset crash-debug
./build/crash-debug/app.exe

- debug
cmake --preset debug
cmake --build --preset debug
./build/debug/app.exe

- clang and uprof
cmake --preset clang
cmake --build --preset clang
./build/clang/app.exe


- tests
cmake --build --preset release --target test-run