#include <exception>
#include <iostream>
#include <memory>
#include "core/store.hpp"
#include "network/server.hpp"
#include "eviction/lru_policy.hpp"
#include "decorators/logging_store.hpp"

int main() {
    try {
        auto base = std::make_shared<miniRedis::Store>(std::make_unique<miniRedis::LruPolicy>());

        auto logged = std::make_shared<miniRedis::LoggingStore>(base);

        miniRedis::TcpServer server(miniRedis::PORT);
        server.setStore(logged);
        server.run();

    } catch (const std::exception& e) {
        std::cerr << " [FATAL] " << e.what() << "\n";
        return 1;
    }
}
