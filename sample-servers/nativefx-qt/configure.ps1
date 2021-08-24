
# configure VCPKG location
$env:VCPKG_ROOT="\dev\vcpkg\"

# configure triplet
$env:VCPKG_TRIPLET="x64-windows"

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="${env:VCPKG_ROOT}\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET="${env:VCPKG_TRIPLET}"