load "io";

class File(filename) {
	set fi = file__openFile(filename);

	form isOpen() {
		return fi != -1;
	}

	form readLine() {
		return file__readLine(fi);
	}

	form readFile() {
		return file__readFile(fi);
	}

	form closeFile() {
		file__closeFile(fi);
		fi = -1;
	}
}
