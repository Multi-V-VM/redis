# Redis

Redis fork ported to WebAssembly and adapted for the Fluence network. More info could be found [here](https://medium.com/fluence-network/porting-redis-to-webassembly-with-clang-wasi-af99b264ca8).

# How to build

This app could be built either with docker 

```bash
docker-compose up
```

or by Makefile with [wasi-sdk](https://github.com/CraneStation/wasi-sdk) installed
```bash
make
```
