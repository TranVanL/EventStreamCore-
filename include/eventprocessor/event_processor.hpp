#pragma once
#include "event/EventBusMulti.hpp"
#include <storage_engine/storage_engine.hpp>
#include <thread>
#include <atomic>
#include "utils/thread_pool.hpp"

class EventProcessor {
public:
    EventProcessor(EventStream::EventBusMulti& bus, StorageEngine& storage, ThreadPool* pool = nullptr)
        : eventBus(bus), storageEngine(storage), workerPool(pool) {}

    virtual ~EventProcessor() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void processLoop() = 0;

protected:
    EventStream::EventBusMulti& eventBus;
    StorageEngine& storageEngine;
    
    std::thread processingThread;
    std::atomic<bool> isRunning{false};
    ThreadPool* workerPool = nullptr;
};
