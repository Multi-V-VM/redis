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

## Prerequisites before deploying to Fluence

The built Redis would have four imports WASI syscalls and before deploying it to the Fluence network, it needs to get rid of them. The most simplest way is to use wasm2wat/wat2wasm utilities from [binaryen](https://github.com/WebAssembly/binaryen):
- translate obtained wasm file to the text representation
```bash
wasm2wat redis.wasm -o redis.wat
``` 
- replace the following strings
```wasm
  (import "wasi_unstable" "fd_fdstat_get" (func $__wasi_fd_fdstat_get (type 1)))
  (import "wasi_unstable" "fd_close" (func $__wasi_fd_close (type 5)))
  (import "wasi_unstable" "fd_seek" (func $__wasi_fd_seek (type 9)))
  (import "wasi_unstable" "fd_write" (func $__wasi_fd_write (type 6)))

```
with
```wasm
(func $__wasi_proc_exit (type 0) (param i32) )
  (func $__wasi_fd_close (type 5) (param i32)(result i32)
    i32.const 0
  )
  (func $__wasi_fd_fdstat_get (type 1) (param i32 i32)(result i32)
    i32.const 0
  )
  (func $__wasi_fd_seek (type 9) (param i32 i64 i32 i32)(result i32)
    i32.const 0
  )
  (func $__wasi_fd_write (type 6) (param i32 i32 i32 i32)(result i32)
    i32.const 0
  )
```
- finally, translate it back to the Wasm:
```bash
wat2wasm redis.wat -o redis.wasm
``` 
