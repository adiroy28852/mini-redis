#pragma once
#include "../core/value.hpp"
#include <string>
#include <vector> 
#include <optional>
#include <chrono>

namespace miniRedis {
    
    class IStore {
    public:
        
        virtual ~IStore() = default;

        virtual void set(const std::string &key, Value val) = 0;
        virtual std::optional<Value> get(const std::string &key) = 0;
        virtual bool del(const std::string &key) = 0;
        virtual bool exists(const std::string &key) = 0;
        virtual std::vector<std::string> keys(const std::string &pattern) = 0;
        virtual void expire(const std::string &k, std::chrono::milliseconds ttl) = 0;
        virtual std::optional<int64_t> ttl(const std::string &key) = 0;
        virtual int64_t incr(const std::string &key) = 0;
        virtual int64_t decr(const std::string &key) = 0;
        virtual size_t dbSize() = 0;
        virtual void flushAll() = 0;
    };
}