ADD_DEFINITIONS(-DTESTDATA_DIR="${PROJECT_SOURCE_DIR}/testdata")

# header tests: these just include specific headers to check if
# they are compilable (e.g., includes and forward declarations are
# complete and do not require extra includes)
FOREACH(HEADER ${LIBRARY_HEADERS})
	STRING(REGEX REPLACE [^a-zA-Z0-9] _ HEADER_NORMALIZED ${HEADER})
	CONFIGURE_FILE(header_test.cc.in ${HEADER_NORMALIZED}_test.cc)
	ADD_EXECUTABLE(${HEADER_NORMALIZED}_test ${HEADER_NORMALIZED}_test.cc)
ENDFOREACH(HEADER ${TESTS})

ADD_EXECUTABLE(binbuf_test binbuf_test.cc)
TARGET_LINK_LIBRARIES(binbuf_test binbuf)
ADD_TEST(binbuf_test binbuf_test)

ADD_EXECUTABLE(string_test string_test.cc)
TARGET_LINK_LIBRARIES(string_test binbuf)
ADD_TEST(string_test string_test)
