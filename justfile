
default:
    just --list

[group('build')]
build:
    cmake -S . -B build
    cmake --build build

[group('run')]
run:
    ./build/EvE