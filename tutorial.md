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
Edit: a `build.zig` file was added, so you can build with just `zig build --search-prefix <path_to_llvm>` now. The executable is placed in (by default) `zig-out/bin/tent`, and core libraries are in `zig-out/lib/`. You'll probably have to add `-S zig-out/lib` to your execution command if you're using core libraries.

Edit: a local installation step has been added to both the `zig build` and CMake build systems.
Assuming you want to install to `~/.local/bin/tent` and `~/.local/lib/tent/*` for the executable and libraries, here are the commands for each build system:
#### CMake
```sh
cmake --build build
make -C build
cmake --install build --prefix ~/.local
```

#### `zig build`
```sh
zig build --prefix ~/.local --search-prefix <path_to_llvm>
```
For `<path_to_llvm>`, you can probably replace it with `$(llvm-config --prefix)`.
There is not yet an uninstall capability for either build system, so to uninstall, you would have to manually delete the executable and library files from the installation path.

## Tutorial!
Let's jump right in! We'll start with the classic "Hello World!". Note that if you want to follow along with this tutorial, you'll need to clone the repo
and build tent using the build instructions above. Without the core libraries, like `io`, you won't be able to use this tutorial.

Anyways, here's "Hello World!" in tent:

```
load "io"; ~ loads core I/O library

println("Hello World!"); ~ prints Hello World
```

Note that comments are denoted using the tilde ('~') character. To run this code, simply run `tent -f <filename>.tent`, and replace `<filename>`
with the filename of your tent file. That's if you want to run it using the evaluator. However, tent also comes with a compiler and VM!
To compile this code, run `tent -c -f <filename>.tent`, replacing `<filename>` with the filename of your tent file once again.

NOTE: LLVM support is being integrated into tent currently, but it's best to stick with the evaluator for now, as the compiler is still a work-in-progress.
