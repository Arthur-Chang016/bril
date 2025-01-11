#ifndef IR_HEAP_H
#define IR_HEAP_H

#include <cstdint>
#include <map>
#include <unordered_map>

namespace ir {

class HeapManager {
   public:
    HeapManager() = default;
    ~HeapManager() = default;
    int64_t *allocate(int size);
    void deallocate(int64_t *ptr);
    bool boundCheck(int64_t *ptr);

   private:
    std::map<int64_t *, int> heap;
};

}  // namespace IR

#endif  // IR_HEAP_H