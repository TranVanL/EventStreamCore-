#pragma once
#include <vector>
#include <mutex>
#include <cstddef>
#include <memory>

class MemoryPool {
public:
    explicit MemoryPool(size_t blockSize, size_t blockCount)
        : blockSize(blockSize), blockCount(blockCount) 
    {
        pool.resize(blockSize * blockCount);
        freeList.reserve(blockCount);

        for (size_t i = 0; i < blockCount; i++) {
            freeList.push_back(pool.data() + i * blockSize);
        }
    }

    void* allocate() {
        std::lock_guard<std::mutex> lock(mu);
        if (freeList.empty()) return nullptr;
        void* p = freeList.back();
        freeList.pop_back();
        return p;
    }

    void deallocate(void* ptr) {
        std::lock_guard<std::mutex> lock(mu);
        freeList.push_back(reinterpret_cast<uint8_t*>(ptr));
    }

private:
    size_t blockSize;
    size_t blockCount;
    std::vector<uint8_t> pool;
    std::vector<uint8_t*> freeList;
    std::mutex mu;
};
