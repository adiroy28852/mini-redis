#pragma once
#include <string>
#include <optional>

namespace miniRedis {
    class IEvictionPolicy {
    public:
        virtual ~IEvictionPolicy() = default;

        //called on every "get" to trac access recency/freq
        virtual void onAccess(const std::string &key) = 0;

        //called on every "set" to register a new key
        virtual void onInsert(const std::string &key) = 0;

        //called on delete/expiration
        virtual void onRemove(const std::string &key) = 0;

        //returns key to evict or null if nothing to evict
        virtual std::optional<std::string> evict() = 0;
    };
}