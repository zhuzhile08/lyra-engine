#include <Common/Array.h>
#include <Common/Dynarray.h>
#include <Common/SharedPointer.h>
#include <Common/Logger.h>
#include <Common/FunctionPointer.h>
#include <Common/FileSystem.h>
#include <Common/Benchmark.h>
#include <Common/JSON.h>

#include <Entity/Entity.h>
#include <Entity/System.h>
#include <Entity/ECS.h>

#include <fmt/core.h>
#include <iostream>
#include <map>

#include <memory>

namespace {

int add(int a, int b) {
	return a + b;
}

}

int main(int argc, char* argv[]) {
	lyra::initFileSystem(argv);
	lyra::initLoggingSystem();
	lyra::initECS();
	
	{ // function pointer test
		lyra::Function<int(int, int)> addFunction(add);
		lyra::log::debug("Result of an addition function stored in a function pointer (w. args): {}\n");
	}

	{ // dynarray test
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
			lyra::log::error("Catched an exception!\n");
		}
	}

	{ // shared pointer test
		lyra::SharedPointer<lyra::uint64> shared1 = lyra::SharedPointer<lyra::uint64>::create(12211411);
		lyra::log::debug("\nShared pointer value and count after construction: {}, {}\n", *shared1, shared1.count());

		lyra::SharedPointer<lyra::uint64> shared2 = shared1;
		*shared2 = 9381295254938;
		lyra::log::debug("Shared pointer value and count after copy and modification: {}, {}", *shared2, shared2.count());

		{
			lyra::SharedPointer<lyra::uint64> shared3 = shared2;
			lyra::log::debug("Unnamed scope shared pointer value and count after copy: {}, {}\n", *shared3, shared3.count());
		}

		lyra::log::debug("Shared pointer count after previous exited scope: {}\n", shared2.count());
	}

	{ // ECS test
		struct ComponentFoo {
			ComponentFoo() = default;
			ComponentFoo(std::string_view eName) : entityName(eName) {
				lyra::log::debug("Component (ComponentFoo) added to Entity!\n");
			}

			std::string entityName;
		};

		struct ComponentBar {
			ComponentBar() {
				lyra::log::debug("Component (ComponentBar) added to Entity!\n");
			}

			void printComponentFooInfo(const lyra::Entity& entity, const ComponentFoo& foo) const {
				lyra::log::warning("System found entity with name: {} and index: {} with both components foo and bar!", foo.entityName, entity.id());
			}
		};

		struct SystemTest {
			void update() {
				system.each([](const lyra::Entity& entity, const ComponentFoo& foo, const ComponentBar& bar){
					bar.printComponentFooInfo(entity, foo);
				});
			}

			lyra::System<ComponentFoo, ComponentBar> system;
		};

		lyra::Entity e("Root");
		e.insert("First")
		 .insert("Second")
		 .insert("Third");

		auto& third = e.child("First::Second::Third");
		auto& foo = third.insert("Foo");
		auto& bar = third.insert("Bar");
		auto& foobar = third.insert("FooBar");

		foo.addComponent<ComponentFoo>(foo.name());
		bar.addComponent<ComponentBar>();

		foobar.addComponent<ComponentFoo>(foobar.name())
			  .addComponent<ComponentBar>();
		
		third.addComponent<ComponentFoo>(third.name())
			 .addComponent<ComponentBar>();

		SystemTest system;
		system.update();
	}

	// lyra::CharVectorStream file("test.json");
	// lyra::Json jsonFile1 = lyra::Json::parse(file.data());

	// lyra::log::info("\nJson Parsing Test: {}\n", jsonFile1.stringify());

	return 0;
}
