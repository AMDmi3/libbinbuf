/*
 * Copyright (c) 2015 Dmitry Marakasov <amdmi3@amdmi3.ru>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef BINFMT_BUFFER_HH
#define BINFMT_BUFFER_HH

#include <binfmt/Config.hh>

#include <memory>
#include <string>

namespace BinFmt {

namespace Internal {
class Chunk;
}

class Buffer;

class Slicable {
public:
	virtual ~Slicable() {}

	virtual Buffer GetSlice(size_t offset = 0, size_t size = -1) const = 0;
	virtual size_t GetSize() const = 0;
};

class Buffer : public Slicable {
private:
	std::shared_ptr<Internal::Chunk> chunk_;
	size_t offset_;
	size_t size_;

private:
	void Realloc(size_t size);
	void ReserveGreedy(size_t needed, size_t desired);

public:
	Buffer(std::shared_ptr<Internal::Chunk> chunk, size_t offset = 0, size_t size = -1);

	Buffer(size_t size = 0);
	virtual ~Buffer();

	Buffer(const Buffer&) = default;
	Buffer& operator=(const Buffer&) = default;

	// Modifiers
	void Reserve(size_t size);
	void Append(Byte data);
	void Append(const Byte* data, size_t size);
	void Append(const Byte* start, const Byte* end);
	void Append(const Slicable& data);

	// Pointer/size getters
	Byte* GetData();
	const Byte* GetData() const;
	virtual size_t GetSize() const override final;
	size_t GetCapacity() const;

	// Subslice
	virtual Buffer GetSlice(size_t offset = 0, size_t size = -1) const override final;

	// Value getters
	Byte operator[](size_t offset) const;

	uint8_t GetU8(size_t offset) const;
	int8_t GetS8(size_t offset) const;

	uint16_t GetU16LE(size_t offset) const;
	int16_t GetS16LE(size_t offset) const;
	uint16_t GetU16BE(size_t offset) const;
	int16_t GetS16BE(size_t offset) const;

	uint32_t GetU32LE(size_t offset) const;
	int32_t GetS32LE(size_t offset) const;
	uint32_t GetU32BE(size_t offset) const;
	int32_t GetS32BE(size_t offset) const;

	std::string GetString(size_t offset = 0, size_t length = std::string::npos) const;
};

}

#endif
