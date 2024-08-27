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

#include <ETCS/Entity.h>
#include <ETCS/System.h>
#include <ETCS/ETCS.h>

#include <fmt/core.h>
#include <iostream>
#include <numeric>
#include <memory>
#include <random>
#include <set>

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
		c1.vec *= glm::vec3(++executionCount);
	}

	static lyra::uint32 executionCount;
};
lyra::uint32 ComponentBar::executionCount = 0;

}

int main(int argc, char* argv[]) {
	lyra::initLoggingSystem();
	etcs::init();

	{ // ECS test
		auto e = etcs::insertEntity();

		auto system = etcs::insertSystem<const ComponentBar, Component1>();

		{
			lyra::Benchmark b;

			for (etcs::object_id i = 0; i < 1000*1000; i++) {
				auto t = etcs::insertEntity();
				
				t.insertComponent<ComponentBar>();
				if (i % 2 == 0) t.insertComponent<Component1>();
				if (i % 3 == 0) {
					t.insertComponent<Component2>();
					t.disable();
				}
				if (i % 4 == 0) t.insertComponent<Component3>();
				if (i % 5 == 0) t.insertComponent<Component4>();
				if (i % 6 == 0) t.insertComponent<Component5>();
			}
		}

		{
			lyra::Benchmark b;
			system.each([](const ComponentBar& bar, Component1 c1){
				bar.update(c1);
			});
		}

		lyra::log::debug("System execution count: {}\n", ComponentBar::executionCount);
	}

	/*
	{ // json parser test
		lsd::Json jsonParse;
		jsonParse = lsd::Json::parse(lsd::String(json));
		lyra::log::info("\nJson Parsing Test: {}\n", jsonParse.stringifyPretty());
	}
	*/

	return 0;
}