` no enums yet so this'll do
set UP_DIRECTION = 'A';
set DOWN_DIRECTION = 'B';
set RIGHT_DIRECTION = 'C';
set LEFT_DIRECTION = 'D';
form get_direction() {
	set c = -1;
	set arrow_key = 0;
	while (c = getc()) != EOF {
		if c == 0x1b {
			arrow_key = 1;
			continue;
		} if arrow_key == 1 && c == '[' {
			arrow_key = 2;
			continue;
		} if arrow_key == 2 && c >= 'A' && c <= 'D' {
			return c;
		} if arrow_key == 0 && (c == 'h' || c == 'a') {
			return LEFT_DIRECTION;
		} if arrow_key == 0 && (c == 'j' || c == 's') {
			return DOWN_DIRECTION;
		} if arrow_key == 0 && (c == 'k' || c == 'w') {
			return UP_DIRECTION;
		} if arrow_key == 0 && (c == 'l' || c == 'd') {
			return RIGHT_DIRECTION;
		}
	}
}
