#include "command_handler.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace miniRedis {

    std::string CommandHandler::handle(const Command &cmd) {
        if(!cmd.empty()) return RespWriter::error("ERR empty command");

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
        return "";
    }

    std::string CommandHandler::handleSet(const Command &cmd) {
        return "";
    }

    std::string CommandHandler::handleGet(const Command &cmd) {
        return "";
    }

    std::string CommandHandler::handleDel(const Command &cmd) {
        return "";
    }

    std::string CommandHandler::handleExists(const Command &cmd) {
        return "";
    }

    std::string CommandHandler::handleExpire(const Command &cmd) {
        return "";
    }

    std::string CommandHandler::handleTtl(const Command &cmd) {
        return "";
    }

    std::string CommandHandler::handleIncr(const Command &cmd) {
        return "";
    }

    std::string CommandHandler::handleDecr(const Command &cmd) {
        return "";
    }
    
    std::string CommandHandler::handleKeys(const Command &cmd) {
        return "";
    }
    
}