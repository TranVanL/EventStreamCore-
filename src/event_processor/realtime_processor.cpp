#include "eventprocessor/realtime_processor.hpp"
#include <spdlog/spdlog.h>
#include <chrono>

RealtimeProcessor::RealtimeProcessor(EventStream::EventBusMulti& bus,
                                     StorageEngine& storage,
                                     ThreadPool* pool)
    : EventProcessor(bus, storage, pool) {}

RealtimeProcessor::~RealtimeProcessor() {
    stop();
}

void RealtimeProcessor::start() {
    isRunning.store(true, std::memory_order_release);
    processingThread = std::thread(&RealtimeProcessor::processLoop, this);
    spdlog::info("RealtimeProcessor started.");
}

void RealtimeProcessor::stop() {
    isRunning.store(false, std::memory_order_release);
    if (processingThread.joinable()) {
        processingThread.join();
    }
    spdlog::info("RealtimeProcessor stopped.");
}

void RealtimeProcessor::processLoop() {
    while (isRunning.load(std::memory_order_acquire)) {
        // Lấy event từ Realtime queue
        auto evtOpt = eventBus.pop(EventStream::EventBusMulti::QueueId::REALTIME,
                                   std::chrono::milliseconds(100));
        if (!evtOpt.has_value()) {
            continue; // timeout, loop lại
        }

        EventStream::EventPtr evtPtr = evtOpt.value();
        if (!evtPtr) continue;

        try {
            if (workerPool) {
                workerPool->submit([evtPtr, this]() {
                    try {
                        // DO something 
                        storageEngine.storeEvent(*evtPtr);
                        spdlog::info("RealtimeProcessor stored event ID {} from source type {}",
                                     evtPtr->header.id,
                                     static_cast<int>(evtPtr->header.sourceType));
                    } catch (const std::exception& e) {
                        spdlog::error("RealtimeProcessor failed to store event ID {}: {}",
                                      evtPtr->header.id, e.what());
                    }
                });
            } else {
                storageEngine.storeEvent(*evtPtr);
                spdlog::info("RealtimeProcessor stored event ID {} from source type {}",
                             evtPtr->header.id,
                             static_cast<int>(evtPtr->header.sourceType));
            }
        } catch (const std::exception& e) {
            spdlog::error("RealtimeProcessor failed to schedule/store event ID {}: {}",
                          evtPtr->header.id, e.what());
        }
    }
}
