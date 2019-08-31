set(CXX_HEADER_FILES
	Lexer.hpp
	Parser.hpp
	Syntax.hpp
	SyntaxTraits.hpp
	Token.hpp
	TokenTraits.hpp
)

set(CXX_SOURCE_FILES
	main.cpp
)

add_executable(Analysis ${CXX_SOURCE_FILES} ${CXX_HEADER_FILES})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Lexer.hpp ${CMAKE_CURRENT_BINARY_DIR}/Lexer.hpp COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Parser.hpp ${CMAKE_CURRENT_BINARY_DIR}/Parser.hpp COPYONLY)
