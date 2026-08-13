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

- Release bez march=native
cmake --preset release-portable
cmake --build --preset release-portable
./build/release-portable/app.exe

- Do lapania crashhandlerem
cmake --preset crash-debug
cmake --build --preset crash-debug
./build/crash-debug/app.exe

- debug
cmake --preset debug
cmake --build --preset debug
./build/debug/app.exe

- profiler - inne buildy nie przechodza w amd uprof
cmake --preset clang
cmake --build --preset clang
./build/clang/app.exe

- testy
cmake --build --preset release --target test-run