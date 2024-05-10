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
#include <numeric>
#include <memory>
#include <random>
#include <set>
#include <unordered_set>

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
		//system.execute([](const lyra::Entity& entity, Component1& c1, Component2& c2, Component3& c3, Component4& c4,  Component5& c5, const ComponentBar& bar){
		//	bar.updateComponents(c1, c2, c3, c4, c5);
		//});
	}

	lyra::System<Component1, Component2, Component3, Component4, Component5, ComponentBar> system;
};

}

int main(int argc, char* argv[]) {
	lyra::initLoggingSystem();
	lyra::initECS();
	
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

			for (lyra::object_id i = 0; i < 65535; i++) {
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
		lyra::Json jsonParse;
		jsonParse = lyra::Json::parse(std::string(json));
		lyra::log::info("\nJson Parsing Test: {}\n", jsonParse.stringifyPretty());
	}

	return 0;
}

*/

// generate a random integer sequence of lenth N
void generateRandomSequence(const size_t N, std::vector<int> & sequence) {

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distr(1, 10000);
    auto draw = std::bind(distr, generator);

    sequence.resize(N);
    for(auto & elem : sequence)
        elem = draw();
}


// find the unique elements in 'sequence' via appending them to a new vector
// and checking before each append inf the element is already in th vector
// Expected complexity: N (loop) * N (find)
void uniqueFind(const std::vector<int> & sequence) {

    std::vector<int> uniques;
    uniques.reserve(sequence.size());
    for(auto elem : sequence) {
        if(std::find(uniques.begin(), uniques.end(), elem) == sequence.end()) {
            uniques.push_back(elem);
        }
    }
    std::sort(uniques.begin(), uniques.end());

}


// find the unique elements in 'sequence' via inserting them into a set and
// then copying the set into a vector
void uniqueSet(const std::vector<int> & sequence) {

    std::set<int> uniquesTmp;
    for(auto elem : sequence) {
        uniquesTmp.insert(elem);
    }
    std::vector<int> uniques(uniquesTmp.begin(), uniquesTmp.end());

}


// find the unique elements in 'sequence' via insering them into a unordered set
// and then copying them into a vector
void uniqueUnordered(const std::vector<int> & sequence) {

    std::unordered_set<int> uniquesTmp;
    for(auto elem : sequence) {
        uniquesTmp.insert(elem);
    }
    std::vector<int> uniques(uniquesTmp.begin(), uniquesTmp.end());
    std::sort(uniques.begin(), uniques.end());

}

// find the unique elements in 'sequence' via std::unique
void unique(const std::vector<int> & sequence) {
    
    std::vector<int> uniques(sequence);
    std::sort(uniques.begin(), uniques.end());
    auto uniqueIt = std::unique(uniques.begin(), uniques.end());
    uniques.erase(uniqueIt, uniques.end());

}




// generate a random integer sequence of lenth N
void generateRandomSequence(const size_t N, lyra::Vector<int> & sequence) {

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distr(1, 10000);
    auto draw = std::bind(distr, generator);

    sequence.resize(N);
    for(auto & elem : sequence)
        elem = draw();
}


// find the unique elements in 'sequence' via appending them to a new vector
// and checking before each append inf the element is already in th vector
// Expected complexity: N (loop) * N (find)
void uniqueFind(const lyra::Vector<int> & sequence) {

    lyra::Vector<int> uniques;
    uniques.reserve(sequence.size());
    for(auto elem : sequence) {
        if(std::find(uniques.begin(), uniques.end(), elem) == sequence.end()) {
            uniques.pushBack(elem);
        }
    }
    std::sort(uniques.begin(), uniques.end());

}


// find the unique elements in 'sequence' via inserting them into a set and
// then copying the set into a vector
void uniqueSet(const lyra::Vector<int> & sequence) {

    lyra::UnorderedSparseSet<int> uniquesTmp;
    for(auto elem : sequence) {
        uniquesTmp.insert(elem);
    }
    lyra::Vector<int> uniques(uniquesTmp.begin(), uniquesTmp.end());

}


// find the unique elements in 'sequence' via insering them into a unordered set
// and then copying them into a vector
void uniqueUnordered(const lyra::Vector<int> & sequence) {

    lyra::UnorderedSparseSet<int> uniquesTmp;
    for(auto elem : sequence) {
        uniquesTmp.insert(elem);
    }
    lyra::Vector<int> uniques(uniquesTmp.begin(), uniquesTmp.end());
    std::sort(uniques.begin(), uniques.end());

}

// find the unique elements in 'sequence' via std::unique
void unique(const lyra::Vector<int> & sequence) {
    
    lyra::Vector<int> uniques(sequence);
    std::sort(uniques.begin(), uniques.end());
    auto uniqueIt = std::unique(uniques.begin(), uniques.end());
    uniques.erase(uniqueIt, uniques.end());

}

// compare the different methods by determining them min time of 'n' runs for a random sequence
// of length 'N'
int main() {
	{
		int n = 10;
		int N = 10000000;
		
		std::vector<size_t> timesFind;
		std::vector<size_t> timesSet;
		std::vector<size_t> timesUnordered;
		std::vector<size_t> times;

		typedef std::chrono::microseconds TimeType;
		
		for(size_t i = 0; i < n; ++i) {
			std::vector<int> sequence;
			generateRandomSequence(N, sequence);
			
			auto startFind = std::chrono::system_clock::now();
			uniqueFind(sequence);
			auto stopFind = std::chrono::system_clock::now();
			auto elapsedFind = std::chrono::duration_cast<TimeType>(stopFind - startFind);
			timesFind.push_back(
				elapsedFind.count()
			);
			
			auto startSet = std::chrono::system_clock::now();
			uniqueSet(sequence);
			auto stopSet = std::chrono::system_clock::now();
			auto elapsedSet = std::chrono::duration_cast<TimeType>(stopSet - startSet);
			timesSet.push_back(
				elapsedSet.count()
			);
			
			auto startUnordered = std::chrono::system_clock::now();
			uniqueUnordered(sequence);
			auto stopUnordered = std::chrono::system_clock::now();
			auto elapsedUnordered = std::chrono::duration_cast<TimeType>(stopUnordered - startUnordered);
			timesUnordered.push_back(
				elapsedUnordered.count()
			);
			
			auto start = std::chrono::system_clock::now();
			unique(sequence);
			auto stop = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<TimeType>(stop - start);
			times.push_back(
				//(elapsed.count())
				elapsed.count()
			);
		}

		std::cout << "Min-time find:" << std::endl;
		std::cout << *std::min_element(timesFind.begin(), timesFind.end()) << std::endl;
		std::cout << "Min-time set:" << std::endl;
		std::cout << *std::min_element(timesSet.begin(), timesSet.end()) << std::endl;
		std::cout << "Min-time unordered:" << std::endl;
		std::cout << *std::min_element(timesUnordered.begin(), timesUnordered.end()) << std::endl;
		std::cout << "Min-time unique:" << std::endl;
		std::cout << *std::min_element(times.begin(), times.end()) << std::endl;
	}

	{
		int n = 10;
		int N = 10000000;
		
		lyra::Vector<size_t> timesFind;
		lyra::Vector<size_t> timesSet;
		lyra::Vector<size_t> timesUnordered;
		lyra::Vector<size_t> times;

		typedef std::chrono::microseconds TimeType;
		
		for(size_t i = 0; i < n; ++i) {
			lyra::Vector<int> sequence;
			generateRandomSequence(N, sequence);
			
			auto startFind = std::chrono::system_clock::now();
			uniqueFind(sequence);
			auto stopFind = std::chrono::system_clock::now();
			auto elapsedFind = std::chrono::duration_cast<TimeType>(stopFind - startFind);
			timesFind.pushBack(
				elapsedFind.count()
			);
			
			auto startSet = std::chrono::system_clock::now();
			uniqueSet(sequence);
			auto stopSet = std::chrono::system_clock::now();
			auto elapsedSet = std::chrono::duration_cast<TimeType>(stopSet - startSet);
			timesSet.pushBack(
				elapsedSet.count()
			);
			
			auto startUnordered = std::chrono::system_clock::now();
			uniqueUnordered(sequence);
			auto stopUnordered = std::chrono::system_clock::now();
			auto elapsedUnordered = std::chrono::duration_cast<TimeType>(stopUnordered - startUnordered);
			timesUnordered.pushBack(
				elapsedUnordered.count()
			);
			
			auto start = std::chrono::system_clock::now();
			unique(sequence);
			auto stop = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<TimeType>(stop - start);
			times.pushBack(
				//(elapsed.count())
				elapsed.count()
			);
		}

		std::cout << "Min-time find:" << std::endl;
		std::cout << *std::min_element(timesFind.begin(), timesFind.end()) << std::endl;
		std::cout << "Min-time set:" << std::endl;
		std::cout << *std::min_element(timesSet.begin(), timesSet.end()) << std::endl;
		std::cout << "Min-time unordered:" << std::endl;
		std::cout << *std::min_element(timesUnordered.begin(), timesUnordered.end()) << std::endl;
		std::cout << "Min-time unique:" << std::endl;
		std::cout << *std::min_element(times.begin(), times.end()) << std::endl;
	}
}
