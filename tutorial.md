# Tent Tutorial

Let's jump right in! We'll start with the classic "Hello World!". Note that if you want to follow along with this tutorial, you'll need to clone the repo and build tent using CMake.
Without the core system libraries, like `io`, you won't be able to use this tutorial.

Anyways, here's "Hello World!" in tent:

```
load "io"; ~ loads core system library

println("Hello World!"); ~ prints Hello World
```

Note that commas are defined using the tilde. To run this code, simply run `tent -f <filename>.tent`, and replace `<filename>` with the filename of your tent file. That's if you
want to run it using the evaluator. However, tent also comes with a compiler and VM! To compile this code, run `tent -c -f <filename>.tent`, replacing `<filename>` with the
filename of your tent file once again. Then, you can run the compiled file with `tent -f <filename>.tnc`, replacing `<filename>` with the filename of your tent file.

The compiler and VM doesn't have as much capability as the evaluator, so for this tutorial, it's best to stick with the evaluator for now. Don't worry, eventually
the compiler and VM will have the same capability as the evaluator, it's something that is still being worked on.
