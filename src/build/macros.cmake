
macro(BuildParserScanner FLEX_L_PREFIX BISON_Y_PREFIX PARSER_PREFIX GENERATED_CPPS)
  ## find our parser generators
  find_program(BISON_EXE bison)
  find_program(FLEX_EXE flex)
  find_program(SED_EXE sed)

  if((BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND")  OR (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))
    # don't have flex/bison/sed, use pregenerated versions
    set (${GENERATED_CPPS} generated/${BISON_Y_PREFIX}.cpp generated/${FLEX_L_PREFIX}.cpp )
  else ((BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND")  OR (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))
    ## build the parser from the flex/yacc sources
    
    ADD_CUSTOM_COMMAND(
      SOURCE "${FLEX_L_PREFIX}.l"
      COMMAND "flex"
      ARGS "-o${FLEX_L_PREFIX}In.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/${FLEX_L_PREFIX}.l"
      OUTPUT ${FLEX_L_PREFIX}In.cpp
      DEPENDS ${FLEX_L_PREFIX}.l
      )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "${FLEX_L_PREFIX}In.cpp"
      COMMAND "sed"
      ARGS -e "'s/${PARSER_PREFIX}wrap(n)/${PARSER_PREFIX}wrap()/g'" -e "'s/yy/${PARSER_PREFIX}/g'" -e "'s/YY/${PARSER_PREFIX}YY/g'"  ${FLEX_L_PREFIX}In.cpp | tee ${FLEX_L_PREFIX}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/generated/${FLEX_L_PREFIX}.cpp > /dev/null
      OUTPUT ${FLEX_L_PREFIX}.cpp
      DEPENDS ${FLEX_L_PREFIX}In.cpp
      )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "${BISON_Y_PREFIX}.y"
      COMMAND "bison"
      ARGS "--defines" "--verbose" "--fixed-output-files" "-p" "${PARSER_PREFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/${BISON_Y_PREFIX}.y"
      OUTPUT y.tab.c y.tab.h
      DEPENDS ${BISON_Y_PREFIX}.y
      )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "y.tab.h"
      COMMAND "sed"
      ARGS -e "'s/yy/${PARSER_PREFIX}/g'" -e "'s/YY/${PARSER_PREFIX}YY/g'" y.tab.h | tee  ${BISON_Y_PREFIX}.tab.h ${CMAKE_CURRENT_SOURCE_DIR}/generated/${BISON_Y_PREFIX}.tab.h > /dev/null
      OUTPUT ${BISON_Y_PREFIX}.tab.h
      DEPENDS y.tab.h
      )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "y.tab.c"
      COMMAND "sed"
      ARGS -e "'s/yy/${PARSER_PREFIX}/g'" -e "'s/YY/${PARSER_PREFIX}YY/g'" y.tab.c | tee ${BISON_Y_PREFIX}.cpp  "${CMAKE_CURRENT_SOURCE_DIR}/generated/${BISON_Y_PREFIX}.cpp" > /dev/null
      OUTPUT ${BISON_Y_PREFIX}.cpp
      DEPENDS y.tab.c ${BISON_Y_PREFIX}.tab.h
      )
    
    ## set build files
    set (${GENERATED_CPPS} ${FLEX_L_PREFIX}.cpp ${BISON_Y_PREFIX}.cpp)
    #add_custom_target(run ALL DEPENDS ${${GENERATED_CPPS}})
  endif( (BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND")  OR (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))
  

endmacro()
