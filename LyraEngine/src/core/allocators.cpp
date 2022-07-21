#include <core/allocators.h>

#include <core/defines.h>

namespace lyra {

// base allocator
BaseAllocator::BaseAllocator(uint32 size, void* start) : _size(size), _start(start) { 
	lassert(_size != 0 && _start != nullptr, "Attemted to create a allocator with invalid starting conditions!");
}

BaseAllocator::~BaseAllocator() {
	lassert(_allocCount == 0 && _usedMemory == 0, "Memory from the allocator at: ", get_address(this), " was not properly deallocated when the destructor of the allocator was called!");
}

// linear allocator
LinearAllocator::~LinearAllocator() {
	_currentPos = nullptr;
}

void* LinearAllocator::alloc(size_t size, uint8 alignment) {
	lassert(_size != 0, "Attemted to allocate memory using a linear allocator at: ", get_address(this), " with a size of 0!");

	// calculate the pointer alignment
	uint8 adjustment = alignPointerAdjustment(_currentPos, alignment);
	uptr alignedAddress = (uptr)_currentPos + adjustment;

	// recalculate the internal variables and check them
	_currentPos = (void*)(alignedAddress + size);
	_usedMemory += size + adjustment;
	if (_usedMemory > size) {	// check if memory is still in bounds
		Logger::log_exception("The allocator at: ", get_address(this), "was instructed to allocate memory with size: ", size, " but does not have enough space left!");
		return nullptr;
	}
	_allocCount += 1;

	return (void*)alignedAddress;
}

void LinearAllocator::clear() {
	_allocCount = 0;
	_usedMemory = 0;
	_currentPos = _start;
}

#ifdef _DEBUG
void LinearAllocator::dealloc(void* p) {
	Logger::log_error("This function is not available for linear allocators and does not exist in the release build. Please call the clear() function if you want to deallocate the memory!");
	clear(); // call the clear function automatically for the user
}
#endif

} // namespace lyra
