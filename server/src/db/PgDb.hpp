//
// Created by ender on 23-7-23.
//

#ifndef ENDER_LABEL_PGDB_HPP
#define ENDER_LABEL_PGDB_HPP

#include "oatpp-postgresql/orm.hpp"
#include "dto/data/ConfigDto.hpp"
#include OATPP_CODEGEN_BEGIN(DbClient)


namespace ender_label::db {
    class PgDb : public oatpp::orm::DbClient {
    public:
        explicit PgDb(const std::shared_ptr<oatpp::orm::Executor> &executor)
            : DbClient(executor) {
            OATPP_COMPONENT(oatpp::Object<dto::data::ConfigDto>, config);
            oatpp::orm::SchemaMigration migration(executor, "ender_label");
            migration.addFile(1, config->migrations + "/table_init.sql");
            migration.migrate();

            OATPP_LOGD("PgDb", "Migration - OK. Version=%ld.", migration.getSchemaVersion());
        }
    };
}

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen

#endif //ENDER_LABEL_PGDB_HPP
