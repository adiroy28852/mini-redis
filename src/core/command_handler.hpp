#pragma once
#include "../decorators/istore.hpp"
#include "../protocol/resp_parser.hpp"
#include "../protocol/resp_writer.hpp"
#include <memory>
#include <string>

namespace miniRedis {

    class CommandHandler {
        std::shared_ptr<IStore> store_;
        std::string handlePing(const Command &cmd);
        std::string handleSet(const Command &cmd);
        std::string handleGet(const Command &cmd);
        std::string handleDel(const Command &cmd);
        std::string handleExists(const Command &cmd);
        std::string handleExpire(const Command &cmd);
        std::string handleTtl(const Command &cmd);
        std::string handleIncr(const Command &cmd);
        std::string handleDecr(const Command &cmd);
        std::string handleKeys(const Command &cmd);
        
    public:
        explicit CommandHandler(std::shared_ptr<IStore> store) : store_(std::move(store)) {}

        std::string handle(const Command &cmd);
    };
}