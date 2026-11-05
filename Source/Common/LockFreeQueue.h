#pragma once

#include <atomic>
#include <array>
#include <cstddef>

namespace GranularLab
{
    struct GrainVisualInfo
    {
        float normPos { 0.0f };       // 0.0 to 1.0 (buffer location)
        float lifeProgress { 0.0f };  // 0.0 to 1.0 (envelope progress)
        float pan { 0.0f };           // -1.0 to 1.0
        float pitchShift { 0.0f };    // semitones
        float amplitude { 0.0f };     // 0.0 to 1.0
        float sizeMs { 50.0f };       // duration
        bool reverse { false };
        bool active { false };
    };

    inline constexpr size_t MAX_VISUAL_GRAINS = 128;

    struct VisualSnapshot
    {
        std::array<GrainVisualInfo, MAX_VISUAL_GRAINS> grains;
        size_t activeCount { 0 };
        float writePosNorm { 0.0f };
        bool isFrozen { false };
    };

    template <typename T, size_t Capacity>
    class SPSCQueue
    {
    public:
        SPSCQueue() : head(0), tail(0) {}

        bool push(const T& item) noexcept
        {
            const size_t currentHead = head.load(std::memory_order_relaxed);
            const size_t nextHead = (currentHead + 1) % Capacity;
            if (nextHead == tail.load(std::memory_order_acquire))
                return false; // Queue is full

            ring[currentHead] = item;
            head.store(nextHead, std::memory_order_release);
            return true;
        }

        bool pop(T& item) noexcept
        {
            const size_t currentTail = tail.load(std::memory_order_relaxed);
            if (currentTail == head.load(std::memory_order_acquire))
                return false; // Queue is empty

            item = ring[currentTail];
            tail.store((currentTail + 1) % Capacity, std::memory_order_release);
            return true;
        }

        bool latest(T& item) noexcept
        {
            bool found = false;
            while (pop(item))
            {
                found = true;
            }
            return found;
        }

        void reset() noexcept
        {
            head.store(0, std::memory_order_relaxed);
            tail.store(0, std::memory_order_relaxed);
        }

    private:
        std::array<T, Capacity> ring;
        std::atomic<size_t> head;
        std::atomic<size_t> tail;
    };
}
