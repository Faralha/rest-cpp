# RESTFUL API using C++

vcpkg must be Installed with Crow header installed.

## Compile locally

Install vcpkg (installation may differ for each machine)

Follow instruction below.

```
https://learn.microsoft.com/en-gb/vcpkg/get_started/get-started?pivots=shell-bash
```

Compile

```bash
  cmake --preset=default
  cmake --build build
```

## Run Server

By default, the server will run on port 8000. Change accordingly.

```bash
  ./build/main
```