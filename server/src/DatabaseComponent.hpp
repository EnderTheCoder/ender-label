//
// Created by ender on 22-8-25.
//

#ifndef ENDER_LABEL_DATABASECOMPONENT_HPP
#define ENDER_LABEL_DATABASECOMPONENT_HPP

#include <oatpp/core/macro/component.hpp>
#include "dto/data/ConfigDto.hpp"
#include "db/PgDb.hpp"
namespace ender_label::component {
    using namespace db;
    class DatabaseComponent {
    public:

        /**
         * Create database client
         */
        OATPP_CREATE_COMPONENT(std::shared_ptr<PgDb>, dbComponent)([] {

            const OATPP_COMPONENT(oatpp::Object<dto::data::ConfigDto>, config); // Get config component

            /* Create database-specific ConnectionProvider */
            const auto connectionProvider = std::make_shared<oatpp::postgresql::ConnectionProvider>(config->db);

            /* Create database-specific ConnectionPool */
            auto connectionPool = oatpp::postgresql::ConnectionPool::createShared(connectionProvider,
                                                                                  1000 /* max-connections */,
                                                                                  std::chrono::seconds(60) /* connection TTL */);

            /* Create database-specific Executor */
            auto executor = std::make_shared<oatpp::postgresql::Executor>(connectionPool);

            /* Create MyClient database client */
            return std::make_shared<PgDb>(executor);

        }());
    };
}
#endif
