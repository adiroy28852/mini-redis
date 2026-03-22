#pragma once

#include <string>
#include <vector>
#include <optional>

namespace miniRedis {
    // resp commands are sent as array of strings
    // ["SET" , "hello", "aditya"]

    struct Command {
        std::vector<std::string> args;
        bool empty() const { return !args.size(); };
        const std::string &name() const { return args[0]; }
    };

    class RespParser {
    public:
        void feed(const char* data, size_t len);
        std::optional<Command> nextCommand();
        bool hasData() const { return !buffer_.empty(); }

    private:
        std::string buffer_;
        // collects data on the go since redis is stateful

        std::optional<std::string> parseBulkString(size_t& pos);
        bool readLine(size_t& pos, std::string& out);
    };
}
