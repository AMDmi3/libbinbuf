# libbinfmt

[![Build Status](https://travis-ci.org/AMDmi3/libbinfmt.svg?branch=master)](https://travis-ci.org/AMDmi3/libbinfmt)

Binary format parsing abstraction library.

## Synopsis

    // Open file for reading; uses mmap if available
    FileContainer file("file.dat");

    // Read 8 byte chunk at offset 0
    Buffer signature = file.GetSlice(0, 8);

    // Extract a string
    assert(signature.GetString() == "MyFmt1.0");
    assert(signature.GetString(0, 8) == "MyFmt1.0");
    assert(signature.GetString(0, 5) == "MyFmt");
    assert(signature.GetString(5, 3) == "1.0");

    // Read another 16 byte chunk at offset 8
    Buffer header = file.GetSlice(8, 16);

    // Extract some values
    uint8_t val1 = header.GetU8(0);     // unsigned byte at 0
    int16_t val2 = header.GetS16LE(1);  // signed litte endian word at 1
    uint32_t val3 = header.GetU32BE(3); // unsigned big endian word at 3

    // Extract sub-chunk
    Buffer entry = header.GetSlice(4, 8);

    // sub-chunks are useful for relative addessing
    // note that entry references the same memory as
    // header, so no copying is performed
    uint8_t = entry[0];  // array-like access

    // This still references the same memory...
    Buffer copy = entry;

    // ...but likely no more after this following line
    copy.Append(';');

    // Yes, may may append data to the buffer - useful if a
    // format needs decompression or decrypting.
    // Library still uses shared buffers for efficiency and
    // convenience, but it always makes sure that one slice
    // modification will never affect other slices.

## Description

This small library implements a low level abstraction layer for
binary file parsing. What it does for you:

- Hides away method of working with a data file. It can either use
  fstream or mmap, and since the latter is more effective, it's used
  when available.
- Hides away method of handling buffers and slices of binary data.
  After you get Buffer (which represents a contiguous chunk of
  bytes) from a container, you can pass and copy it around, create
  slices of it, but internally no data copying is performed - all
  Buffers will reference a single place in memory. When mmap container
  is used, it's not even copied from mapped memory.
- Allows easy extraction of strings and integer values of different
  sizes, signedness and endianess

## Usage ##

To build libbinfmt, you need a compiler with C++11 support, for
example clang 3.4+ or gcc 4.8+, and cmake.

To build standalone version:

    cmake . && make

You may either bundle this library with your project (if you use
cmake, just add this to your CMakeLists.txt:

    ADD_SUBDIRECTORY(extlib/libbinfmt) # where libbinfmt code is

    ...

    INCLUDE_DIRECTORIES(${BINFMT_INCLUDE_DIRS})

    ADD_EXECUTABLE(mytarget ...)
    TARGET_LINK_LIBRARIES(mytarget ${BINFMT_LIBRARIES})

) or intall it systemwide with

    make install

## Author ##

* [Dmitry Marakasov](https://github.com/AMDmi3) <amdmi3@amdmi3.ru>

## License ##

2-clause BSD, see COPYING.
