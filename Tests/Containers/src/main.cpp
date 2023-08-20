#include <Common/Array.h>
#include <Common/Dynarray.h>
#include <Common/Logger.h>
#include <Common/FunctionPointer.h>
#include <Common/FileSystem.h>
#include <Common/Benchmark.h>
#include <Json/Json.h>

#include <fmt/core.h>
#include <iostream>

namespace {

int add(int a, int b) {
	return a + b;
}

}

int main(int argc, char* argv[]) {
	lyra::initFileSystem(argv);
	
	lyra::Function<int(int, int)> addFunction(add);
	lyra::log::debug("Result of an addition function stored in a function pointer (w. args): {}\n");

	lyra::Dynarray<int, 16> foo;
	foo.resize(10);
	foo.fill(4);

	for (const auto& it : foo) lyra::log::log("{}{} ", lyra::ansi::setStyle(lyra::ansi::Font::none, 242), it);
	lyra::log::newLine();
	lyra::log::debug("After insert:");
	foo.insert(foo.begin() + 5, 5);

	for (const auto& it : foo) lyra::log::log("{}{} ", lyra::ansi::setStyle(lyra::ansi::Font::none, 242), it);
	lyra::log::newLine();
	lyra::log::debug("After inserting multiple elements:");
	foo.insert(foo.begin() + 6, 2, 6);

	for (const auto& it : foo) lyra::log::log("{}{} ", lyra::ansi::setStyle(lyra::ansi::Font::none, 242), it);
	lyra::log::newLine();
	lyra::log::debug("After erasing one element:");
	foo.erase(foo.begin() + 9);

	for (const auto& it : foo) lyra::log::log("{}{} ", lyra::ansi::setStyle(lyra::ansi::Font::none, 242), it);
	lyra::log::newLine();
	lyra::log::debug("After erasing multiple elements:");
	foo.erase(foo.begin() + 6, foo.end() - 1);

	for (const auto& it : foo) lyra::log::log("{}{} ", lyra::ansi::setStyle(lyra::ansi::Font::none, 242), it);
	lyra::log::newLine();
	lyra::log::debug("After inserting multiple elements and causing an exception:");
	try {
		foo.insert(foo.begin(), 30, 8);
	} catch(...) {
		lyra::log::exception("Catched an exception!\n");
	}

	lyra::CharVectorStream file("test.json");
	lyra::Json jsonFile1 = lyra::Json::parse(file.data());

	lyra::log::info("Json Parsing Test: {}\n", jsonFile1.stringify());

	return 0;
}
