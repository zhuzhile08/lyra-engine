#include <core/allocators.h>



namespace lyra {

// base allocator
BaseAllocator::BaseAllocator(uint32 size, void* start) : m_size(size), m_start(start) { 
	lassert(m_size != 0 && m_start != nullptr, "Attemted to create a allocator with invalid starting conditions!");
}

BaseAllocator::~BaseAllocator() {
	lassert(m_allocCount == 0 && m_usedMemory == 0, "Memory from the allocator at: ", get_address(this), " was not properly deallocated when the destructor of the allocator was called!");
}

// linear allocator
LinearAllocator::~LinearAllocator() {
	m_currentPos = nullptr;
}

void* LinearAllocator::alloc(size_t size, uint8 alignment) {
	lassert(m_size != 0, "Attemted to allocate memory using a linear allocator at: ", get_address(this), " with a size of 0!");

	// calculate the pointer alignment
	uint8 adjustment = alignPointerAdjustment(m_currentPos, alignment);
	uptr alignedAddress = (uptr)m_currentPos + adjustment;

	// recalculate the internal variables and check them
	m_currentPos = (void*)(alignedAddress + size);
	m_usedMemory += size + adjustment;
	if (m_usedMemory > size) {	// check if memory is still in bounds
		Logger::log_exception("The allocator at: ", get_address(this), "was instructed to allocate memory with size: ", size, " but does not have enough space left!");
		return nullptr;
	}
	m_allocCount += 1;

	return (void*)alignedAddress;
}

void LinearAllocator::clear() {
	m_allocCount = 0;
	m_usedMemory = 0;
	m_currentPos = m_start;
}

#ifndef NDEBUG
void LinearAllocator::dealloc(void* p) {
	Logger::log_error("This function is not available for linear allocators and does not exist in the release build. Please call the clear() function if you want to deallocate the memory!");
	clear(); // call the clear function automatically for the user
}
#endif

} // namespace lyra
