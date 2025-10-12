load "io";

class Name(value) {
    aura = value.length * 3;
}

class User(name) {
    println(name.aura);
}

User(Name("John"));