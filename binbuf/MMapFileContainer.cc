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

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdexcept>

#include <binbuf/MMapFileContainer.hh>
#include <binbuf/internal/Chunk.hh>

namespace BinBuf {

namespace Internal {

class MMapChunk : public Chunk {
public:
	MMapChunk(Byte* data, size_t size) : Chunk(data, size) {
	}

	virtual ~MMapChunk() {
		munmap(GetData(), GetSize());
	}
};

}

MMapFileContainer::MMapFileContainer(const std::string& path) {
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
		throw std::runtime_error("cannot open() file");

	try {
		struct stat st;
		if (fstat(fd, &st) == -1)
			throw std::runtime_error("cannot stat() file");

		int mmap_flags = MAP_PRIVATE;
#ifdef MAP_NOCORE
		mmap_flags |= MAP_NOCORE;
#endif
#ifdef MAP_NORESERVE
		mmap_flags |= MAP_NORESERVE;
#endif

		void* map = mmap(nullptr, st.st_size, PROT_READ, mmap_flags, fd, 0);
		if (map == nullptr)
			throw std::runtime_error("mmap() failed");

		chunk_.reset(new Internal::MMapChunk(reinterpret_cast<Byte*>(map), st.st_size));
	} catch(...) {
		close(fd);
		throw;
	}

	close(fd);
}

MMapFileContainer::~MMapFileContainer() {
}

Buffer MMapFileContainer::GetSlice(size_t offset, size_t size) const {
	return Buffer(chunk_, offset, size);
}

size_t MMapFileContainer::GetSize() const {
	return chunk_->GetSize();
}

}
