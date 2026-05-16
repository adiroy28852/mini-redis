#include "command_handler.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace miniRedis {

    std::string CommandHandler::handle(const Command &cmd) {
        if (!cmd.empty())
            return RespWriter::error("ERR empty command");

        std::string name = cmd.name();
        std::transform(begin(name), end(name), begin(name), ::toupper);

        if (name == "PING") return handlePing(cmd);
        if (name == "SET") return handleSet(cmd);
        if (name == "GET") return handleGet(cmd);
        if (name == "DEL") return handleDel(cmd);
        if (name == "EXISTS") return handleExists(cmd);
        if (name == "EXPIRE") return handleExpire(cmd);
        if (name == "TTL") return handleTtl(cmd);
        if (name == "INCR") return handleIncr(cmd);
        if (name == "DECR") return handleDecr(cmd);
        if (name == "KEYS") return handleKeys(cmd);
        if (name == "DBSIZE") return RespWriter::integer(store_->dbSize());
        if (name == "FLUSHALL") { store_->flushAll(); return RespWriter::ok(); }

        return RespWriter::error("ERR unknown command '" + cmd.name() + "'");
    }

    std::string CommandHandler::handlePing(const Command &cmd) {
        // blank PING = PONG
        // PING "abcde" = echo back "abcde"
        if (cmd.args.size() == 1)
            return RespWriter::bulkString(cmd.args[1]);
        return RespWriter::pong();
    }

    std::string CommandHandler::handleSet(const Command& cmd) {
        if (cmd.args.size() < 3)
            return RespWriter::error("ERR wrong number of arguments for SET");

        Value val = Value::fromString(cmd.args[2]);

        // Parse optional flags: SET key value EX 60  |  SET key value PX 500
        for (size_t i = 3; i + 1 < cmd.args.size(); i += 2) {
            std::string flag = cmd.args[i];
            std::transform(flag.begin(), flag.end(), flag.begin(), ::toupper);
            if (flag == "EX") {
                int secs = std::stoi(cmd.args[i + 1]);
                val.expiry = std::chrono::steady_clock::now() + std::chrono::seconds(secs);
            } else if (flag == "PX") {
                int ms = std::stoi(cmd.args[i + 1]);
                val.expiry = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
            }
        }

        store_->set(cmd.args[1], std::move(val));
        return RespWriter::ok();
    }

    std::string CommandHandler::handleGet(const Command& cmd) {
        if (cmd.args.size() < 2)
            return RespWriter::error("ERR wrong number of arguments for GET");

        auto val = store_->get(cmd.args[1]);
        if (!val) return RespWriter::null();

        const std::string* s = val->asString();
        if (!s) return RespWriter::error("WRONGTYPE value is not a string");
        return RespWriter::bulkString(*s);
    }

    std::string CommandHandler::handleDel(const Command& cmd) {
        if (cmd.args.size() < 2)
            return RespWriter::error("ERR wrong number of arguments for DEL");
        int deleted = 0;
        for (size_t i = 1; i < cmd.args.size(); ++i)
            if (store_->del(cmd.args[i])) ++deleted;
        return RespWriter::integer(deleted);
    }

    std::string CommandHandler::handleExists(const Command& cmd) {
        if (cmd.args.size() < 2)
            return RespWriter::error("ERR wrong number of arguments for EXISTS");
        return RespWriter::integer(store_->exists(cmd.args[1]) ? 1 : 0);
    }

    std::string CommandHandler::handleExpire(const Command& cmd) {
        if (cmd.args.size() < 3)
            return RespWriter::error("ERR wrong number of arguments for EXPIRE");
        int secs = std::stoi(cmd.args[2]);
        store_->expire(cmd.args[1], std::chrono::milliseconds(secs * 1000));
        return RespWriter::integer(1);
    }

    std::string CommandHandler::handleTtl(const Command& cmd) {
        if (cmd.args.size() < 2)
            return RespWriter::error("ERR wrong number of arguments for TTL");
        auto result = store_->ttl(cmd.args[1]);
        return RespWriter::integer(result ? *result : -2);
    }

    std::string CommandHandler::handleIncr(const Command& cmd) {
        if (cmd.args.size() < 2)
            return RespWriter::error("ERR wrong number of arguments for INCR");
        try {
            return RespWriter::integer(store_->incr(cmd.args[1]));
        } catch (const std::runtime_error& e) {
            return RespWriter::error(e.what());
        }
    }

    std::string CommandHandler::handleDecr(const Command& cmd) {
        if (cmd.args.size() < 2)
            return RespWriter::error("ERR wrong number of arguments for DECR");
        try {
            return RespWriter::integer(store_->decr(cmd.args[1]));
        } catch (const std::runtime_error& e) {
            return RespWriter::error(e.what());
        }
    }

    std::string CommandHandler::handleKeys(const Command& cmd) {
        std::string pattern = (cmd.args.size() > 1) ? cmd.args[1] : "*";
        auto result = store_->keys(pattern);
        return RespWriter::array(result);
    }
}