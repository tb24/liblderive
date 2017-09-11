# Need bison/flex
find_package(BISON)
find_package(FLEX)

BISON_TARGET(MyParser  ${CMAKE_CURRENT_SOURCE_DIR}/parser.lsys.y ${CMAKE_CURRENT_SOURCE_DIR}/y.tab.c)
FLEX_TARGET(MyScanner ${CMAKE_CURRENT_SOURCE_DIR}/lexer.lsys.l ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.c)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

# Find source files
file(GLOB SOURCES *.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_library(lderive SHARED ${SOURCES} ${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS})
