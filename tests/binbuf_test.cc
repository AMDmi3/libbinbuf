#include <binbuf/MMapFileContainer.hh>
#include <binbuf/StreamFileContainer.hh>

#include <vector>

#include "testing.h"

using namespace BinBuf;

BEGIN_TEST()
	// test mmap container itself
	MMapFileContainer mmap(TESTDATA_DIR "/testfile");

	// test slice read from mmap container
	Buffer mmap_slice = mmap.GetSlice();

	// test slice of that slice
	Buffer mmap_slice_slice = mmap_slice.GetSlice();

	// test copy of a slice
	Buffer mmap_slice_copy = mmap_slice;

	// test slices appended in different ways
	Buffer appended_all, appended_bytewise;
	appended_all.Append(mmap_slice.GetData(), mmap_slice.GetSize());

	for (const Byte* c = mmap_slice.GetData(); c != mmap_slice.GetData() + mmap_slice.GetSize(); ++c)
		appended_bytewise.Append(*c);

	// test slices of larger buffer
	Buffer front_slice, back_slice;
	front_slice.Reserve(15);
	front_slice.Append((Byte)'-');
	front_slice.Append(mmap_slice);
	front_slice = front_slice.GetSlice(1, 14);

	back_slice.Reserve(15);
	back_slice.Append(mmap_slice);
	back_slice.Append((Byte)'-');
	back_slice = back_slice.GetSlice(0, 14);

	// test file containeritself
	StreamFileContainer file(TESTDATA_DIR "/testfile");

	std::vector<Slicable*> sources = {
		&mmap,
		&mmap_slice,
		&mmap_slice_slice,
		&mmap_slice_copy,
		&appended_all,
		&appended_bytewise,
		&front_slice,
		&back_slice,
		&file
	};

	for (auto& source : sources) {
		// check size
		EXPECT_EQUAL(source->GetSize(), (size_t)14);

		// check subslices
		EXPECT_EQUAL(source->GetSlice().GetSize(), (size_t)14);
		EXPECT_EQUAL(source->GetSlice(0).GetSize(), (size_t)14);
		EXPECT_EQUAL(source->GetSlice(14).GetSize(), (size_t)0);
		EXPECT_EQUAL(source->GetSlice(0, 14).GetSize(), (size_t)14);
		EXPECT_EQUAL(source->GetSlice(0, 1).GetSize(), (size_t)1);
		EXPECT_EQUAL(source->GetSlice(13, 1).GetSize(), (size_t)1);
		EXPECT_EQUAL(source->GetSlice(14, 1).GetSize(), (size_t)0);

		// get slice covering whole file
		Buffer data = source->GetSlice();
		EXPECT_EQUAL(data.GetSize(), (size_t)14);

		// byte getters
		EXPECT_EQUAL((int)data[0], 0x01);
		EXPECT_EQUAL((int)data[1], 0x23);

		EXPECT_EQUAL((int)data.GetU8(3), 103);
		EXPECT_EQUAL((int)data.GetS8(3), 103);
		EXPECT_EQUAL((int)data.GetU8(7), 239);
		EXPECT_EQUAL((int)data.GetS8(7), -17);

		// word getters
		EXPECT_EQUAL((int)data.GetU16LE(1), 0x4523);
		EXPECT_EQUAL((int)data.GetU16BE(1), 0x2345);
		EXPECT_EQUAL((int)data.GetU16LE(5), 0xCDAB);
		EXPECT_EQUAL((int)data.GetU16BE(5), 0xABCD);

		EXPECT_EQUAL((int)data.GetS16LE(1), 17699);
		EXPECT_EQUAL((int)data.GetS16BE(1), 9029);
		EXPECT_EQUAL((int)data.GetS16LE(5), -12885);
		EXPECT_EQUAL((int)data.GetS16BE(5), -21555);

		// dword getters
		EXPECT_EQUAL(data.GetU32LE(0), 0x67452301U);
		EXPECT_EQUAL(data.GetU32BE(0), 0x01234567U);
		EXPECT_EQUAL(data.GetU32LE(4), 0xEFCDAB89U);
		EXPECT_EQUAL(data.GetU32BE(4), 0x89ABCDEFU);

		EXPECT_EQUAL(data.GetS32LE(0), 1732584193);
		EXPECT_EQUAL(data.GetS32BE(0), 19088743);
		EXPECT_EQUAL(data.GetS32LE(4), -271733879);
		EXPECT_EQUAL(data.GetS32BE(4), -1985229329);

		// string getters
		EXPECT_EQUAL(data.GetString(8), "FooBar");
		EXPECT_EQUAL(data.GetString(8, 6), "FooBar");
	}
END_TEST()
