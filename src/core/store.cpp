#include "store.hpp"
#include <stdexcept>
#include <regex>

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

public:

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

    /**
exists — if exists then 1 else 0.
keys — returns all non-expired keys matching a glob pattern - i think i need to learn regex
expire — sets a TTL on an existing key. after that, delete.
ttl —  how many seconds until a key expires. Returns -1 if no expiry set, -2 if key doesn't exist.
incr — increments a key's integer value by 1. create if not exist. error if type!= int.
decr — subtracts 1. Creates at -1 if missing.
dbSize — basicallly v.size() i hve to implement
flushAll — v.clear();
sweepExpired — actively scans and deletes all expired keys. Lazy expiry (in get) handles individual keys on access — this handles keys that nobody ever reads again.
     */
}