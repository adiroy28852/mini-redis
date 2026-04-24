#include <iostream>
#include "decorators/logging_store.hpp"
#include "decorators/metrics_store.hpp"
#include "core/store.hpp"
#include "eviction/lru_policy.hpp"
#include "network/server.hpp"

int main() {
    // make the store stack
    auto base = std::make_shared<miniRedis::Store>(
        std::make_unique<miniRedis::LRUPolicy>()
    );
    auto logged = std::make_shared<miniRedis::LoggingStore>(base);
    auto metrics = std::make_shared<miniRedis::MetricsStore>(logged);


    //server uses the outermost layer. no idea about underneath layers
    miniRedis::TcpServer server(miniRedis::PORT);
    server.setStore(metrics);
    server.run();
}
