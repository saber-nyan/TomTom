Для разработки используется JetBrains CLion, GCC 9.2.0, CMake 3.14.

Работает под GNU/Linux с glibc, требует:
* `libwebsockets` (встроено)
* `flatcc` (встроено)
* `libpq`
* `pthreads`
* `openssl`
* `zlib`

Под Debian зависимости качаются как
```
# apt update
# apt install -y --no-install-recommends gcc make libpq-dev libc-dev zlib1g-dev libssl-dev postgresql-server-dev-11 wget ca-certificates
# wget -O- 'https://cmake.org/files/LatestRelease/cmake-3.15.5-Linux-x86_64.tar.gz' | tar --strip-components=1 -xz -C /usr/
```

## Сборка

```bash
$ git clone ...
$ mkdir tomtom-build; cd tomtom-build
$ cmake -DCMAKE_BUILD_TYPE=Release ../backend  # Или -DCMAKE_BUILD_TYPE=Debug
$ make -j4 

$ ./tomtom_backend -v
```

***

*desu~*
