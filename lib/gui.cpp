#include "native.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>

#if defined(__linux__)
#include <fontconfig/fontconfig.h>
#endif

static std::unique_ptr<Fl_Window> globalWindow;

Value createWindow(const std::vector<Value>& args) {
	if (args.size() < 3) return Value();

	int width = std::get<nl_int_t>(args[0].v);
	int height = std::get<nl_int_t>(args[1].v);
	std::string title = std::get<std::string>(args[2].v);

	globalWindow = std::make_unique<Fl_Window>(width, height, title.c_str());
	globalWindow->copy_label(title.c_str());

	return Value((nl_int_t)1);
}

Value showWindow(const std::vector<Value>&) {
	if (globalWindow) {
		globalWindow->end();
		globalWindow->show();
	}
	
	return Value((nl_int_t)1);
}

Value createButton(const std::vector<Value>& args) {
	if (!globalWindow || args.size() < 5) return Value();

	int x = std::get<nl_int_t>(args[0].v);
	int y = std::get<nl_int_t>(args[1].v);
	int w = std::get<nl_int_t>(args[2].v);
	int h = std::get<nl_int_t>(args[3].v);
	std::string label = std::get<std::string>(args[4].v);

	Fl_Button* btn = new Fl_Button(x, y, w, h, label.c_str());
	btn->copy_label(label.c_str());
	globalWindow->add(btn);

	return Value((nl_int_t)1);
}

Value runGUI(const std::vector<Value>&) {
#if defined(__linux__)
	FcInit();
#endif

	Fl::run();
	globalWindow.reset();
	
#if defined(__linux__)
	FcFini();
#endif
	
	return Value((nl_int_t)1);
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
	table["createWindow"] = createWindow;
	table["showWindow"] = showWindow;
	table["createButton"] = createButton;
	table["runGUI"] = runGUI;
}