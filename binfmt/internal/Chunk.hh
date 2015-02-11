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

#ifndef BINFMT_CHUNK_HH
#define BINFMT_CHUNK_HH

#include <binfmt/Config.hh>

namespace BinFmt {

namespace Internal {

class Chunk {
private:
	Byte* data_;
	size_t size_;
	const size_t capacity_;

protected:
	Chunk(Byte* data, size_t size) : data_(data), size_(size), capacity_(size) {
	}

	Chunk(Byte* data, size_t size, size_t capacity) : data_(data), size_(size), capacity_(capacity) {
	}

public:
	virtual ~Chunk() {
	}

	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;
	Chunk(Chunk&&) = delete;
	Chunk& operator=(Chunk&&) = delete;

	Byte* GetData() { return data_; }
	const Byte* GetData() const { return data_; }
	size_t GetSize() const { return size_; }
	size_t GetCapacity() const { return capacity_; }

	void Append(Byte data) { data_[size_++] = data; }
	void Append(const Byte* data, size_t size) {
		std::copy(data, data + size, data_ + size_);
		size_ += size;
	}
};

}

}

#endif
