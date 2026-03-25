#pragma once
#include <string>
#include <vector> 
#include <variant>
#include <chrono>
#include <optional>

namespace miniRedis {
    using ValueData = std::variant<
        std::string, 
        int64_t,
        std::vector<std::string>
    >;

    struct Value {
        ValueData data;
        std::optional<std::chrono::steady_clock::time_point> expiry;

        bool isExpired() const {
            if(!expiry) return false;
            return std::chrono::steady_clock::now() >= *expiry;
        }
        // factory of helpers
        static Value fromString(std::string s) {
            return { std::move(s), {} };
        }

        static Value fromInt(int64_t n) {
            return { n, {} };
        }

        const std::string* asString() const {
            return std::get_if<std::string>(&data);
        }

        int64_t* asInt() {
            return std::get_if<int64_t>(&data);
        }
    };
}