# Tent ⛺️

Tent is an interpreted programming language that has many features. The aim is to have a syntax that's recognizable, but different. To use tent, simply clone the project,
and then you can build the project through CMake. There will eventually be releases of tent, each release containing the source compiled executable, system libraries, and full source
code. To start learning tent, you can start with the [tutorial](tutorial.md).

## NEW!

Tent now supports LLVM for compilation! In order to test it out, try this sample code:

```
print("Hello World!");
```

Compile it with: `./tent compile <path to tent file>`. You should see the generated executable `t.out`!

## Update with LLVM

While loops, if then else statements, and the break and continue statements are now supported. Note that variables are still not quite ready yet, but are on the way!
Also, note that unfortunately, tent can only compile on Unix-based systems (Linux, MacOS, etc.); we're still working on support for Windows.
