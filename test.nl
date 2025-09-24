form test(num) {
    if false {
        println("Hello World!");
        return num;
    }

    println("This is also executed!");

    return num;
}

println(test(5));