# Need bison/flex
find_package(BISON)
find_package(FLEX)

BISON_TARGET(MyParser parser.lsys.y ${CMAKE_CURRENT_BINARY_DIR}/y.tab.c)
FLEX_TARGET(MyScanner lexer.lsys.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

include_directories(lderive ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# For flex/bison output 
include_directories(lderive ${CMAKE_CURRENT_BINARY_DIR})
include_directories(lderive ${CMAKE_CURRENT_SOURCE_DIR})

add_library(lderive 
	Colors.c
	Common.c
	Defines.c
	Errors.c
	Evaluator.c
	HLSystem.c
	ILSystem.c
	LEngine.c
	LGL.c
	LinkedList.c
	LRuleNeighbors.c
	LSContext.c
	LSModules.c
	LString.c
	LSystem.c
	LTurtle.c
	LVbo.c
	Memos.c
	Parser.c
	Semantics.c
	Stack.c
	SymbolTable.c
	TurtleInterpreter.c
	Vector.c
	${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS}
	)
