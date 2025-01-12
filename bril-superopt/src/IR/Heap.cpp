#include <IR/Heap.h>

#include <format>
#include <stdexcept>

namespace ir {

int64_t *HeapManager::allocate(int size) {
    if (size <= 0) throw std::runtime_error("error: must allocate a positive amount of memory: " + std::to_string(size));
    int64_t *ptr = new int64_t[size];
    heap[ptr] = size;
    return ptr;
}

void HeapManager::deallocate(int64_t *ptr) {
    if (auto f = heap.find(ptr); f != heap.end()) {
        delete[] ptr;
        heap.erase(f);
    } else {
        throw std::runtime_error("Base addr not found in heap: " + std::format("0x{:x}", reinterpret_cast<uintptr_t>(ptr)));
    }
}

// true only if valid
bool HeapManager::boundCheck(int64_t *ptr) {
    auto greater = heap.upper_bound(ptr);
    if (greater == heap.begin()) return false;
    auto [base, size] = *std::prev(greater);  // less than or equal to ptr
    return ptr < base + size && ptr >= base;
}

}  // namespace ir
