cmake_minimum_required(VERSION 3.24.0)
project(Containers VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 23)

include_directories(
PRIVATE
	${LIBRARY_PATH}/glm/
	# library local include directory
	${LYRA_INCLUDE_DIR}
)

# build the content manager
add_executable(Containers
	"src/main.cpp"
)
