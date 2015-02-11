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

#include <binfmt/Buffer.hh>

#include <binfmt/internal/ManagedChunk.hh>

#include <algorithm>

namespace BinFmt {

Buffer::Buffer(std::shared_ptr<Internal::Chunk> chunk, size_t offset, size_t size) : chunk_(chunk), offset_(offset), size_(size) {
	size_t real_size = chunk_->GetSize();
	if (offset_ > real_size)
		offset_ = real_size;
	if (size_ > real_size - offset_)
		size_ = real_size - offset_;
}

Buffer::Buffer(size_t size) : chunk_(new Internal::ManagedChunk(size)) {
}

Buffer::~Buffer() {
}

// Modifiers
void Buffer::Realloc(size_t size) {
	Internal::ManagedChunk* chunk = new Internal::ManagedChunk(0, size);
	chunk->Append(chunk_->GetData() + offset_, std::min(size, size_));
	chunk_.reset(chunk);
	offset_ = 0;
}

void Buffer::Reserve(size_t size) {
	ReserveGreedy(size, size);
}

void Buffer::ReserveGreedy(size_t needed, size_t desired) {
	// we'll not shrink
	if (needed < size_)
		needed = size_;
	if (desired < needed)
		desired = needed;

	// ensure chunk is appendable (slice ends where chunk used space ends, e.g.
	// there's no data possibly used by other slice we may override) and that
	// there's enough space
	if (offset_ + size_ != chunk_->GetSize() || needed > chunk_->GetCapacity() - offset_)
		Realloc(desired);
}

void Buffer::Append(Byte data) {
	Append(&data, 1);
}

void Buffer::Append(const Byte* data, size_t size) {
	ReserveGreedy(size_ + size, std::max(size_ + size, size_ * 2));
	chunk_->Append(data, size);
	size_ += size;
}

void Buffer::Append(const Byte* start, const Byte* end) {
	Append(start, end - start);
}

void Buffer::Append(const Slicable& data) {
	Buffer slice = data.GetSlice();
	Append(slice.GetData(), slice.GetSize());
}

// Pointer/size getters
Byte* Buffer::GetData() {
	return chunk_->GetData() + offset_;
}

const Byte* Buffer::GetData() const {
	return chunk_->GetData() + offset_;
}

size_t Buffer::GetSize() const {
	return size_;
}

size_t Buffer::GetCapacity() const {
	return chunk_->GetCapacity() - offset_;
}

// subslice
Buffer Buffer::GetSlice(size_t offset, size_t size) const {
	if (offset > size_)
		offset = size_;
	if (size > size_ - offset)
		size = size_ - offset;

	return Buffer(chunk_, offset_ + offset, size);
}

Byte Buffer::operator[](size_t offset) const {
	const Byte* offptr = chunk_->GetData() + offset_ + offset;
	return *offptr;
}

uint8_t Buffer::GetU8(size_t offset) const {
	const Byte* offptr = chunk_->GetData() + offset_ + offset;
	return (uint8_t)*offptr;
}

int8_t Buffer::GetS8(size_t offset) const {
	const Byte* offptr = chunk_->GetData() + offset_ + offset;
	return (int8_t)*offptr;
}

uint16_t Buffer::GetU16LE(size_t offset) const {
	const Byte* offptr = chunk_->GetData() + offset_ + offset;
	return (uint16_t)*(offptr)
		| ((uint16_t)*(offptr + 1) << 8);
}

int16_t Buffer::GetS16LE(size_t offset) const {
	return (int16_t)GetU16LE(offset);
}

uint16_t Buffer::GetU16BE(size_t offset) const {
	const Byte* offptr = chunk_->GetData() + offset_ + offset;
	return ((uint16_t)(uint8_t)*(offptr) << 8)
		| (uint16_t)(uint8_t)*(offptr + 1);
}
int16_t Buffer::GetS16BE(size_t offset) const {
	return (int16_t)GetU16BE(offset);
}

uint32_t Buffer::GetU32LE(size_t offset) const {
	const Byte* offptr = chunk_->GetData() + offset_ + offset;
	return (uint32_t)*(offptr)
		| ((uint32_t)*(offptr + 1) << 8)
		| ((uint32_t)*(offptr + 2) << 16)
		| ((uint32_t)*(offptr + 3) << 24);
}

int32_t Buffer::GetS32LE(size_t offset) const {
	return (int32_t)GetU32LE(offset);
}

uint32_t Buffer::GetU32BE(size_t offset) const {
	const Byte* offptr = chunk_->GetData() + offset_ + offset;
	return ((uint32_t)*(offptr) << 24)
		| ((uint32_t)*(offptr + 1) << 16)
		| ((uint32_t)*(offptr + 2) << 8)
		| (uint32_t)*(offptr + 3);
}

int32_t Buffer::GetS32BE(size_t offset) const {
	return (int32_t)GetU32BE(offset);
}

std::string Buffer::GetString(size_t offset, size_t length) const {
	if (offset > size_)
		offset = size_;
	if (length > size_ - offset)
		length = size_ - offset;
	const Byte* offptr = chunk_->GetData() + offset_ + offset;
	return std::string(reinterpret_cast<const char*>(offptr), length);
}

}
