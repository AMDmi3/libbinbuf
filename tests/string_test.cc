#include <binfmt/FileContainer.hh>

#include "testing.h"

using namespace BinFmt;

BEGIN_TEST()
	FileContainer container(TESTDATA_DIR "/strings");

	// this is for padding trimming feature which I may add later
	std::string str1 = container.GetSlice(0, 8).GetString();
	EXPECT_EQUAL(str1.size(), (size_t)8);

	std::string str2 = container.GetSlice(0, 8).GetString(0, 8);
	EXPECT_EQUAL(str2.size(), (size_t)8);

	// zero bytes should not get into the string
	std::string str3 = container.GetSlice(8, 8).GetString();
	EXPECT_EQUAL(str3.size(), (size_t)4);

	std::string str4 = container.GetSlice(8, 8).GetString(0, 8);
	EXPECT_EQUAL(str4.size(), (size_t)4);
END_TEST()
