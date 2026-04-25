# Tent Tutorial
## Building
First, you'll have to build the `tent` program. This project uses the CMake build system, so these are the commands (you can skip this if you already know what to do):
```sh
# in project root directory
mkdir -p build
cmake --build build
make -C build
```
The `tent` executable should be available in `build/tent`. You can leave it there or move it elsewhere (I suggest moving it to the project root to follow along with this tutorial, as it will
assume you've done this). This will also build all the core libraries you'll need.
The command to run the `tent` program may differ depending on your operating system:
- MacOS/Linux: `./tent`
- Windows: `.\tent` <br />

This will be written as `tent` from here on for brevity.

Edit: a local installation step has been added to the CMake build systems.
Assuming you want to install to `~/.local/bin/tent` and `~/.local/lib/tent/*` for the executable and libraries, here are the commands:
#### CMake
```sh
cmake --build build
make -C build
cmake --install build --prefix ~/.local
```

There is not yet an uninstall capability for either build system, so to uninstall, you would have to manually delete the executable and library files from the installation path.

## Tutorial!
Let's jump right in! We'll start with the classic "Hello World!". Note that if you want to follow along with this tutorial, you'll need to clone the repo
and build tent using the build instructions above. Without the core libraries, like `io`, you won't be able to use this tutorial.

Anyways, here's "Hello World!" in tent:

```
load "io"; ~ loads core I/O library

println("Hello World!"); ~ prints Hello World
```

Note that comments are denoted using the tilde ('~') character. To run this code, simply run `tent <filename>.tent`, and replace `<filename>`
with the filename of your tent file. That's it.
