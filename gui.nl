load "gui";

form run() {
	runGUI();
}

class Window(width, height, title) {
	createWindow(width, height, title);

	form show() {
		showWindow();
	}
}

class Button(x, y, width, height, label) {
	createButton(x, y, width, height, label);
}
