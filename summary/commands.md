# Najważniejsze komendy

## Clean

```bash
rm -rf build

- Release
cmake --preset release
cmake --build --preset release
./build/release/app.exe

- Release bez march=native
cmake --preset release-portable
cmake --build --preset release-portable
./build/release-portable/app.exe

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