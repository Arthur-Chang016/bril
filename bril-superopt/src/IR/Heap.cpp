#include <IR/Heap.h>

#include <stdexcept>

namespace ir {

int64_t *HeapManager::allocate(int size) {
    int64_t *ptr = new int64_t[size];
    heap[ptr] = size;
    return ptr;
}

void HeapManager::deallocate(int64_t *ptr) {
    if (auto f = heap.find(ptr); f != heap.end()) {
        delete[] ptr;
        heap.erase(f);
    } else {
        throw std::runtime_error("Pointer not found in heap");
    }
}

bool HeapManager::boundCheck(int64_t *ptr) {
    auto greater = heap.upper_bound(ptr);
    if (greater == heap.begin()) return false;
    auto [base, size] = *std::prev(greater);  // less than or equal to ptr
    return ptr < base + size && ptr >= base;
}

}  // namespace IR