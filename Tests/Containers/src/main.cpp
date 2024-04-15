#include <Common/Array.h>
#include <Common/Dynarray.h>
#include <Common/Vector.h>
#include <Common/UnorderedSparseMap.h>
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

#include <numeric>

#include <memory>

namespace {

constexpr const char* json("\
{ \
	\"First Loose Variable Test\": \"Hello, World!\",\
\
	\"Structure Test\": {\
		\"String\": \"cFRzGjjQPs%UQK@jRutx\",\
		\"Floating Point\": 3.1415926,\
		\"Nestled Structure Test\": {\
			\"Unsigned Integer\": 23450908\
		},\
		\"Nestled Array Test\": [\
			159807.234\
		],\
		\"Signed Integer\": -485038\
	},\
\
	\"Array Test\": [\
		\"cc1UjRB*q6BRY1&MWUk0\",\
		\"xePtYYW=Mm&rKQ8mQtf1\",\
		\"+1OkC4QafUb%46ptJprU\",\
		\"O*%1Kn!%x#KCeUs4Qa1z\"\
	],\
\
	\"Structure inside Array Test\": [\
		{\
			\"Exponent\": 2954.8e-7,\
			\"String\": \"h09vJ+SvsKpDPoP6ZbfJ\"\
		},\
		{\
			\"Joke\": \"Why did the chicken cross the road? Because it wanted to get to the other side\",\
			\"Fact\": \"The brain is one of the organs in the human body with the highest percent of fat\",\
			\"Sentence\": \"Kept you waiting, huh?\"\
		}\
	],\
\
	\"Second Loose Variable Test\": \"Goodbye, World!\"\
}\
");

class Test : public lyra::Node<Test> {
public:
	using lyra::Node<Test>::BasicNode;
};

struct Component1 {
	glm::vec2 v;
};

struct Component2 {
	glm::vec2 v;
};

struct Component3 {
	glm::vec2 v;
};

struct Component4 {
	glm::vec2 v;
};

struct Component5 {
	glm::vec2 v;
};

struct ComponentBar {
	ComponentBar() = default;

	void updateComponents(Component1& c1, Component2& c2, Component3& c3, Component4& c4,  Component5& c5) const {
		c1.v = {1, 1};
		c2.v = {2, 2};
		c3.v = {3, 3};
		c4.v = {4, 4};
		c5.v = {5, 5};
		executionCount++;
	}

	static lyra::uint32 executionCount;
};
lyra::uint32 ComponentBar::executionCount = 0;

struct SystemTest {
	void update() {
		system.each([](const lyra::Entity& entity, Component1& c1, Component2& c2, Component3& c3, Component4& c4,  Component5& c5, const ComponentBar& bar){
			bar.updateComponents(c1, c2, c3, c4, c5);
		});
	}

	lyra::System<Component1, Component2, Component3, Component4, Component5, ComponentBar> system;
};

}

int main(int argc, char* argv[]) {
	lyra::initLoggingSystem();
	lyra::initECS();

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
		lyra::Entity e("Root");

		{
			lyra::Benchmark b;

			for (lyra::objectid i = 0; i < 65535; i++) {
				auto& t = e.emplace(std::to_string(i));
				
				t.addComponent<ComponentBar>();
				if (i % 2 == 0) t.addComponent<Component1>();
				if (i % 3 == 0) t.addComponent<Component2>();
				if (i % 4 == 0) t.addComponent<Component3>();
				if (i % 5 == 0) t.addComponent<Component4>();
				if (i % 6 == 0) t.addComponent<Component5>();
			}
		}

		{
			lyra::Benchmark b;

			SystemTest system;
			system.update();
		}

		lyra::log::debug("System execution count: {}\n", ComponentBar::executionCount);
	}

	{ // json parser test
		lyra::Json jsonParse = lyra::Json::parse(std::string(json));
		lyra::log::info("\nJson Parsing Test: {}\n", jsonParse.stringifyPretty());
	}

	return 0;
}
