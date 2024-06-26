# SHA-1

A simple SHA-1 implementation in C.

## Building the binary

```
$ ./build.sh        # with optimizations (no warnings)
$ ./build.sh dev    # with warnings
$ ./build.sh tests  # run the few tests that exists
```

This builds the binary `sha1`.

## Usage

```
$ echo -n "Hello World!" | sha1    # Pipe with data
$ cat README.md | sha1
$ sha1 README.md .clangd-format    # Hash file(s)
```

## TODO

- [ ] Windows build
