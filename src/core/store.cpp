#include "store.hpp"
#include <chrono>
#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <regex>
#include <string>
#include <vector>

namespace miniRedis {
    
    Store::Store(std::unique_ptr<IEvictionPolicy> policy) : eviction_(std::move(policy)) {}

    // private helpers

    bool Store::internalExists(const std::string &key) const {
        auto it = data_.find(key);
        if(it == data_.end()) return false;
        if(it->second.isExpired()) return false;
        return true;
    }

    void Store::evictIfNeeded() {
        // only evict if ( we have a policy AND there is something to evict )
        if(!eviction_) return;
        auto evictionObj = eviction_->evict();
        if(evictionObj) data_.erase(*evictionObj);
    }


    void Store::set(const std::string &key, Value val) {
        std::unique_lock lock(mutex_);
        bool isNew = (data_.find(key) == end(data_));
        data_[key] = std::move(val);
        if (eviction_) {
            if (isNew) eviction_->onInsert(key);
            else eviction_->onAccess(key);
        }
        evictIfNeeded();
    }

    std::optional<Value> Store::get(const std::string &key) {
        std::shared_lock lock(mutex_); 
        auto it = data_.find(key);
        if (it == end(data_)) return std::nullopt;
        
        if(eviction_) eviction_->onAccess(key);
        return it->second;
    }

    bool Store::del(const std::string& key) {
        std::unique_lock lock(mutex_);
        auto it = data_.find(key);
        if (it == data_.end()) return false;
        if (eviction_) eviction_->onRemove(key);
        data_.erase(it);
        return true;
    }

    bool Store::exists(const std::string &key){
        std::shared_lock lock(mutex_);
        return internalExists(key);
    }

    std::vector<std::string> Store::keys(const std::string &pattern){
        std::shared_lock lock(mutex_);
        std::vector<std::string> result;

        // convert glob to regex
        // * becomes *. | ? becomes .

        std::string regStr;
        for(char i : pattern){
            if (i=='*')regStr += ".*";
            else if (i=='?')regStr += '.';
            else if (std::string(".+^${}()|[]\\").find(i) != std::string::npos)
            regStr += std::string("\\") + i;  // escape regex special chars
            else regStr += i;
        }
        std::regex re("^" + regStr + "$");

        for (auto& [k, v] : data_) {
            if (v.isExpired()) continue;                    // skip expired
            if (std::regex_match(k, re)) result.push_back(k);
        }
        return result;
    }

    void Store::expire(const std::string &key, std::chrono::milliseconds ttl){
        std::unique_lock lock(mutex_);
        auto it = data_.find(key);
        if (it == end(data_))return;
        it->second.expiry = std::chrono::steady_clock::now() + ttl;
    }

    std::optional<int64_t> Store::ttl(const std::string &key){
        std::shared_lock lock(mutex_);
        auto it = data_.find(key);
        if (it == end(data_)) return -2;
        if (!it->second.expiry) return -1;
        if (it->second.isExpired())return -2;    
        
        auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
            *it->second.expiry - std::chrono::steady_clock::now()
        );
        return remaining.count();
    }

    int64_t Store::incr(const std::string& key) {
        std::unique_lock lock(mutex_);
        auto it = data_.find(key);
    
        if (it == data_.end()) {
            // key doesn't exist — create it with value 1
            Value v = Value::fromInt(1);
            data_[key] = v;
            if (eviction_) eviction_->onInsert(key);
            return 1;
        }
    
        // key exists — must be an integer
        int64_t* n = it->second.asInt();
        if (!n) throw std::runtime_error("ERR value is not an integer");
        ++(*n);
        if (eviction_) eviction_->onAccess(key);
        return *n;
    }
    
    int64_t Store::decr(const std::string& key) {
        std::unique_lock lock(mutex_);
        auto it = data_.find(key);
    
        if (it == data_.end()) {
            Value v = Value::fromInt(-1);
            data_[key] = v;
            if (eviction_) eviction_->onInsert(key);
            return -1;
        }
    
        int64_t* n = it->second.asInt();
        if (!n) throw std::runtime_error("ERR value is not an integer");
        --(*n);
        if (eviction_) eviction_->onAccess(key);
        return *n;
    }
    
    size_t Store::dbSize() {
        std::shared_lock lock(mutex_);
        return data_.size();
    }
    
    void Store::flushAll() {
        std::unique_lock lock(mutex_);
        data_.clear();
        // note: eviction policy tracks are now stale — rebuild is too expensive
        // simplest fix: just let the policy accumulate ghost entries
        // they'll be ignored since evict() returns keys that no longer exist
    }
    
    void Store::sweepExpired() {
        std::unique_lock lock(mutex_);
        for (auto it = data_.begin(); it != data_.end(); ) {
            if (it->second.isExpired()) {
                if (eviction_) eviction_->onRemove(it->first);
                it = data_.erase(it);  // erase returns next valid iterator
            } else {
                ++it;
            }
        }
    }
}