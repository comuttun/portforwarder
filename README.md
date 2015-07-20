# PortForwarder

## The original website

http://toh.fuji-climb.org/pf/

## License

http://toh.fuji-climb.org/pf/licence.html

## Prerequisite

* Microsoft Visual Studio 2013
* Multibyte MFC Library for Visual Studio 2013
* CMake

## How to build

### 1. Clone Repository

1. `git clone --recursive https://github.com/comutt/portforwarder.git`
2. `cd portforwarder`

### 2. OpenSSL

1. `cd openssl`
2. `perl Configure VC-WIN32`
3. `ms\do_ms`
4. Replace `/MD` with `/MT` in ms\nt.mak
5. `nmake -f ms\nt.mak`
6. `cd ..`

### 3. zlib

1. `cd zlib`
2. `cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_C_FLAGS_DEBUG=/MTd -DCMAKE_C_FLAGS_RELEASE=/MT .`
3. `msbuild ALL_BUILD.vcxproj /p:configuration=Release`
4. `cd ..`

### 4. PortForwarder

1. `msbuild PortForwarder.vcxproj /p:configuration=Release`
