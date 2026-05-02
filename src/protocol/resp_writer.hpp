#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace miniRedis {
    struct RespWriter {
        static std::string ok() { return "+OK\r\n"; }
        static std::string pong() { return "+PONG\r\n"; }
        static std::string null() { return "$-1\r\n"; }
        static std::string error(const std::string &msg) { return "-ERR " + msg + "\r\n"; }
        static std::string integer(int64_t n) { return ":" + std::to_string(n) + "\r\n"; }

        static std::string bulkString(const std::string &s) { return "$" + std::to_string(s.size()) + "\r\n" + s + "\r\n"; }

        static std::string array(const std::vector<std::string> &items) {
            std::string res = "*" + std::to_string(items.size()) + "\r\n";
            for(auto &item : items) res+=bulkString(item);
            return res;
        }
    };
}