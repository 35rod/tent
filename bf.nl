~ an implementation of a brainf*ck interpreter, in newLang

load "io.dylib";
load "stdnl.dylib";
load "../file.nl";

if ARG_COUNT < 1 {
	println("usage: bf <filename>");
	exit();
}
set DATA_SIZE = 32768;

set source_file = File(ARGS@0);
set instr = source_file.readFile();
source_file.closeFile();
set instr_pos = 0;			~ instruction pointer

set data = vec_from_size(DATA_SIZE); ~ data
set data_pos = 0;				~ data pointer

set n_nested = 0;
set save_nested = n_nested;

while instr_pos < (instr.length - 1) {
	if instr@instr_pos == "#" {
		while instr@instr_pos != "\n"
			instr_pos++;
		++instr_pos;
		continue;
	} else if instr@instr_pos == ">" {
		++data_pos;
		if data_pos >= DATA_SIZE
			data_pos -= DATA_SIZE;
	} else if instr@instr_pos == "<" {
		--data_pos;
		if data_pos < 0
			data_pos += DATA_SIZE;
	} else if instr@instr_pos == "+" {
		data@data_pos = data@data_pos + 1;
	} else if instr@instr_pos == "-" {
		data@data_pos = data@data_pos - 1;
	} else if instr@instr_pos == "." {
		print(chr(data@data_pos));
	} else if instr@instr_pos == "," {
		data@data_pos = getc();
	} else if instr@instr_pos == "[" && data@data_pos == 0 {
		save_nested = n_nested;
		++n_nested;
		while save_nested != n_nested {
			++instr_pos;
			if instr@instr_pos == "[" {
				++n_nested;
			} else if instr@instr_pos == "]" {
				--n_nested;
			}
		}
	} else if instr@instr_pos == "]" && data@data_pos != 0 {
		save_nested = n_nested;
		++n_nested;
		while save_nested != n_nested {
			--instr_pos;
			if instr@instr_pos == "]" {
				++n_nested;
			} else if instr@instr_pos == "[" {
				--n_nested;
			}
		}
	}

	instr_pos++;
}
