#pragma once
#include "istore.hpp"
#include <memory>
#include <chrono>

namespace miniRedis {
    
    class LoggingStore : public IStore {
        
        std::shared_ptr<Istore> inner_;
        void log(const std::string &op, const std::string &key){
            std::cout<< "[LOG] " << op << ' ' << key << "\n";
        }

    public:
        explicit LoggingStore(std::shared_ptr<IStore> inner) : inner_(std::move(inner)) {}
        void set(const std::string &key, Value val) override {
            log("SET", key);
            inner_ -> set(key, std::move(val));
        }

        std::optional<Value> get(const std::string &key) override {
            auto t0 = std::chrono::steady_clock::now();
            auto res = inner_ -> get(key);
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - t0
            ).count();

            std::cout << "[LOG] GET " << key << (res ? " HIT" : " MISS") << " (" << us << "microsec)\n";
            return res;
        }
        
        bool del(const std::string &key) override {
            log("DEL", key);
            return inner_ -> del(key);
        }
        bool exists(const std::string &key) override {
            return inner_ -> exists(key);
        }
        std::vector<std::string> keys(const std::string &p) override {
            return inner_ -> keys(p);
        }
        void expire(const std::string &k, std::chrono::milliseconds t) override {
            inner_ -> expire(k, t);
        }
        std::optional<int_64_t> ttl(const std::string &k) override {
            return inner_ -> ttl(k);
        }
        int64_t incr(const std::string &k) override {
            return inner_ -> incr(k);
        }
        int64_t decr(const std::string &k) override {
            return inner_ -> decr(k);
        }
        size_t dbSize() override {
            return inner_ -> dbSize();
        }
        void flushAll() override {
            inner_ -> flushAll();
        }
    }
}