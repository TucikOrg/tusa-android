//
// Created by Artem on 12.10.2024.
//

#ifndef TUSA_ANDROID_CIRCULARBUFFER_H
#define TUSA_ANDROID_CIRCULARBUFFER_H

#include <iostream>
#include <unordered_map>
#include <vector>

template<typename Key, typename Value>
class CircularBuffer {
public:
    CircularBuffer(size_t capacity) : capacity(capacity), start(0), size(0) {
        buffer.resize(capacity);
    }

    // Add a new element, removing the oldest if the buffer is full
    void add(const Key& key, const Value& value) {
        // If the key already exists, update the value and return
        if (lookup.find(key) != lookup.end()) {
            size_t index = lookup[key];
            buffer[index].second = value;
            return;
        }

        // If the buffer is full, remove the oldest element
        if (size == capacity) {
            const Key& oldest_key = buffer[start].first;
            lookup.erase(oldest_key); // Remove the oldest from the hashmap
            start = (start + 1) % capacity; // Move start to the next element
        } else {
            size++;
        }

        // Add the new element at the current end of the buffer
        size_t end = (start + size - 1) % capacity;
        buffer[end] = {key, value};
        lookup[key] = end; // Add to hashmap for fast lookup
    }

    // Get an element by key (returns a pointer, nullptr if not found)
    Value* get(const Key& key) {
        auto it = lookup.find(key);
        if (it != lookup.end()) {
            return &buffer[it->second].second;
        }
        return nullptr;
    }

    std::vector<std::pair<Key, Value>> getBuffer() {
        return buffer;
    }

private:
    size_t capacity;
    size_t start;
    size_t size;
    std::vector<std::pair<Key, Value>> buffer; // Circular buffer storage
    std::unordered_map<Key, size_t> lookup; // Fast lookup by key
};


#endif //TUSA_ANDROID_CIRCULARBUFFER_H
