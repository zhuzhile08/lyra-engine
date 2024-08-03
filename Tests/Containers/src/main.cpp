#include <Common/Logger.h>
#include <Common/FileSystem.h>
#include <Common/Benchmark.h>

#include <LSD/Array.h>
#include <LSD/Dynarray.h>
#include <LSD/Vector.h>
#include <LSD/String.h>
#include <LSD/UnorderedSparseMap.h>
#include <LSD/SharedPointer.h>
#include <LSD/FunctionPointer.h>
#include <LSD/Format.h>
#include <LSD/JSON.h>

#include <ECS/Entity.h>
#include <ECS/System.h>
#include <ECS/ECS.h>

#include <fmt/core.h>
#include <iostream>
#include <numeric>
#include <memory>
#include <random>
#include <set>

/*
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

struct Component1 {
	glm::vec3 vec;
};

struct Component2 {
	char m[64];
};

struct Component3 {
	char m[64];
};

struct Component4 {
	char m[64];
};

struct Component5 {
	char m[64];
};

struct ComponentBar {
	ComponentBar() = default;

	void update(Component1& c1) const {
		c1.vec = glm::vec3(executionCount);

		executionCount++;
	}

	static lyra::uint32 executionCount;
};
lyra::uint32 ComponentBar::executionCount = 0;

}

int main(int argc, char* argv[]) {
	lyra::initLoggingSystem();
	lyra::initECS();

	{ // ECS test
		lyra::Entity e("Root");

		lyra::System<const ComponentBar, Component1> system;
		system.each(
			[](const ComponentBar& bar, Component1 c1){
			bar.update(c1);
		});

		{
			lyra::Benchmark b;

			for (lyra::object_id i = 0; i < 1000*1000; i++) {
				auto& t = e.emplace(lsd::toString(i));
				
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
			system.run();
		}

		lyra::log::debug("System execution count: {}\n", ComponentBar::executionCount);
	}

	{ // json parser test
		lsd::Json jsonParse;
		jsonParse = lsd::Json::parse(lsd::String(json));
		lyra::log::info("\nJson Parsing Test: {}\n", jsonParse.stringifyPretty());
	}

	return 0;
}
*/

int main() {
	//std::cout << std::format( "{:<20}\n", "left");
	//std::cout << std::format( "{:>20}\n", "right");
	//std::cout << std::format( "{:^20}\n", "centered");
	std::cout << fmt::format("{{}}{{}} {{}} {}", "fuckery");
}
