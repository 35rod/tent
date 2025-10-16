# Tent Tutorial
## Building
First, you'll have to build the `tent` program. This project uses the CMake build system, so these are the commands (you can skip this if you already know what to do):
```sh
# in project root directory
mkdir -p build
cmake -B build
make -C build
```
The `tent` executable should be available in `build/tent`. You can leave it there or move it elsewhere (I suggest moving it to the project root to follow along with this tutorial, as it will
assume you've done this). This will also build all the core libraries you'll need.
The command to run the `tent` program may differ depending on your operating system:
- MacOS/Linux: `./tent`
- Windows: `.\tent`
This will be written as `tent` from here on for brevity.
Edit: a `build.zig` file was added, so you can build with just `zig build` now. The executable is placed in (by default) `zig-out/bin/tent`, and core libraries are in `zig-out/lib/`. You'll
probably have to add `-S zig-out/lib` to your execution command if you're using core libraries.


## Tutorial!
Let's jump right in! We'll start with the classic "Hello World!". Note that if you want to follow along with this tutorial, you'll need to clone the repo
and build tent using CMake (see build instructions above). Without the core libraries, like `io`, you won't be able to use this tutorial.

Anyways, here's "Hello World!" in tent:

```
load "io"; ~ loads core I/O library

println("Hello World!"); ~ prints Hello World
```

Note that comments are denoted using the tilde ('~') character. To run this code, simply run `tent -f <filename>.tent`, and replace `<filename>`
with the filename of your tent file. That's if you want to run it using the evaluator. However, tent also comes with a compiler and VM!
To compile this code, run `tent -c -f <filename>.tent`, replacing `<filename>` with the filename of your tent file once again. Then, you can run
the compiled file with `tent -f <filename>.tnc`, replacing `<filename>` with the filename of your tent file.

The compiler and VM doesn't have as much capability as the evaluator, so for this tutorial, it's best to stick with the evaluator for now. Don't worry, eventually
the compiler and VM will have the same capability as the evaluator, it's something that is still being worked on.
