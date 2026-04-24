#pragma once
#include "../decorators/istore.hpp"
#include "../eviction/eviction_policy.hpp"
#include <unordered_map>
#include <memory>
#include <shared_mutex>

namespace miniRedis {

    class Store : public IStore {
        
        std::unordered_map<std::string, Value> data_;
        std::unique_ptr<IEvictionPolicy> eviction_;
        mutable std::shared_mutex mutex_;

        bool internalExists(const std::string& key) const;
        void evictIfNeeded();

    
    public:
        explicit Store(std::unique_ptr<IEvictionPolicy> policy = nullptr);
        void set(const std::string &key, Value val) override;
        std::optional<Value> get(const std::string& key) override;
        bool del(const std::string& key) override;
        bool exists(const std::string& key) override;
        std::vector<std::string> keys(const std::string& pattern) override;
        void expire(const std::string& k, std::chrono::milliseconds ttl) override;
        std::optional<int64_t> ttl(const std::string& key) override;
        int64_t incr(const std::string &key) override;
        int64_t decr(const std::string &key) override;
        size_t dbSize() override;
        void flushAll() override;

        void sweepExpired();
    };
}