set board = [" ", " ", " ", " ", " ", " ", " ", " ", " "];

form print_board() {
	println(board@0 + "|" + board@1 + "|" + board@2);
	println(board@3 + "|" + board@4 + "|" + board@5);
	println(board@6 + "|" + board@7 + "|" + board@8);
}

set turns = 0;

while turns < 9 {
	print_board();

	set current_turn = 10;

	if turns % 2 == 0 {
		println("It is X's turn");

		set spot = stoll(input("Choose a spot (1-9): ")) - 1;

		if (board@spot == " ") {
			vecassign(spot, "board", "X");
			current_turn = ++turns;
		}
	} if turns % 2 == 1 {
		if turns != current_turn {
			println("It is O's turn");

			set spot = stoll(input("Choose a spot (1-9): ")) - 1;

			if (board@spot == " ") {
				vecassign(spot, "board", "O");
				++turns;
			}
		}
	}
}
