set(CXX_SOURCE_FILES
	main.cpp
)

set(CXX_HEADER_FILES
	defines.hpp
	int_ptr.hpp
	Buffer.hpp
	ScopedNode.hpp
	ScopedNode.Iterator.hpp
	Tree.hpp
	VectorNode.hpp
)

add_executable(Utils ${CXX_SOURCE_FILES} ${CXX_HEADER_FILES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Buffer.hpp ${CMAKE_CURRENT_BINARY_DIR}/Buffer.hpp COPYONLY)
