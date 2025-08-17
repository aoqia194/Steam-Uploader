# Building with CMake

Building in release mode (single-configuration generator):

```sh
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build
```

Building in release mode (multi-configuration generator):

```sh
cmake -S . -B build
cmake --build build --config Release
```
