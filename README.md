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

### 1. OpenSSL

1. `git clone https://github.com/comutt/PortForwarder.git`
2. `cd PortForwarder/openssl`
3. `perl Configure VC-WIN32`
4. `ms\do_ms`
5. Replace `/MD` with `/MT` in ms\nt.mak
6. `nmake -f ms\nt.mak`

### 2. zlib

1. `cd zlib`
2. Add

        set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /MT")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")

3. `cmake .`

### 3. PortForwarder

Coming soon (maybe)
