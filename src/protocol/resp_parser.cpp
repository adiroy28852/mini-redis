#include "resp_parser.hpp"
#include <stdexcept>
// RESP = redis serialization protocol
namespace miniRedis {

    void RespParser::feed(const char* data, size_t len) {
        buffer_.append(data, len);
    }

    bool RespParser::readLine(size_t& pos, std::string& out){
        auto crlf = buffer_.find("\r\n", pos);
        if(crlf==std::string::npos)return false;
        out = buffer_.substr(pos, crlf - pos);
        pos = crlf + 2;
        return true;
    }
    std::optional<Command> RespParser::nextCommand() {
        if(buffer_.empty())return std::nullopt;
        if(buffer_[0] != '*')return std::nullopt;

        size_t pos = 0;
        std::string line;
        if(!readLine(pos, line)) return std::nullopt;

        std::int count = std::stoi(line.substr(1));
        if(count <= 0)return std::nullopt;

        Command cmd;
        for(int i = 0; i<count; i++) {
            if(!token) return std::nullopt;

            cmd.args.push_back(std::move(*token));
        }

        buffer_.erase(0, pos);
        return cmd;
    }

    std::optional<std::string> RespParser::parseBulkString(size_t& pos) {
        if(pos >= buffer_.size() or buffer_[pos] != '$') return std::nullopt;
        std::string lenLine;
        if(!readLine(pos, lenLine)) return std::nullopt;

        int len = std::stoi(lenLine.substr(1));
        if (len == -1)return std::string{};

        if(pos + len + 2 > buffer_.size())return std::nullopt;

        std::string value = buffer_.substr(pos, len);
        pos += len + 2;
        return value;;
    }
}