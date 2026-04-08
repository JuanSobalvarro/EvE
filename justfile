
default:
    just --list

[group('build')]
build:
    cmake -S . -B build
    cmake --build build

[group('build')]
build_run: build
    ./build/EvE
    
[group('run')]
run:
    ./build/EvE