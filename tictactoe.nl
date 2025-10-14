load "io";
load "stdnl";
load "math";

board = vec.fill(9, " ");

form print_board() {
    println(board@0 + "|" + board@1 + "|" + board@2);
    println(board@3 + "|" + board@4 + "|" + board@5);
    println(board@6 + "|" + board@7 + "|" + board@8);
}

if rand() > 0.5 {
    first = "O";
    second = "X";
} else {
    first = "X";
    second = "O";
}

turns = 0;

while turns < 9 {
    print_board();

    if turns % 2 == 0 {
        println("It is " + first + "'s turn.");

        spot = stoi(input("Select a spot (1-9): ")) - 1;

        if board@spot == " " {
            board@spot = first;
            turns++;
        }
    } else {
        println("It is " + second + "'s turn.");

        spot = stoi(input("Select a spot (1-9): ")) - 1;

        if board@spot == " " {
            board@spot = second;
            turns++;
        }
    }
}

print_board();