#pragma once
#include "eviction_policy.hpp"
#include <list>
#include <unordered_map>

namespace miniRedis {

    class LruPolicy : public IEvictionPolicy {
        std::list<std::string> order_;
        std::unordered_map<std::string, std::list<std::string>::iterator> map_;
    public:
        void onAccess(const std::string &key) override {
            auto it = map_.find(key);
            if(it == end(map_)) return;
            order_.splice(order_.begin(), order_, it->second); // move to front O(1)
        }

        void onInsert(const std::string &key) override {
            order_.push_front(key);
            map_[key] = order_.begin();
        }

        void onRemove(const std::string &key) override {
            auto it = map_.find(key);
            if(it == end(map_)) return;
            order_.erase(it->second);
            map_.erase(it);
        }

        std::optional<std::string> evict() override {
            if(order_.empty()) return std::nullopt;
            std::string victim = order_.back();
            onRemove(victim);
            return victim;
        }
    };
}