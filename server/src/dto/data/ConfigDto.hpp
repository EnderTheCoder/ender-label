//
// Created by ender on 23-7-23.
//

#ifndef ENDER_LABEL_CONFIGDTO_HPP
#define ENDER_LABEL_CONFIGDTO_HPP

#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto {
    class ConfigDto final : public oatpp::DTO {
        DTO_INIT(ConfigDto, oatpp::DTO)
        DTO_FIELD(String, host);
        DTO_FIELD(UInt16, port);
        DTO_FIELD(String, db);
        DTO_FIELD(String, resources);
        DTO_FIELD(String, storage);
        DTO_FIELD(Int32, cache_time);
        DTO_FIELD(String, redis);
        DTO_FIELD(String, config_type);
        DTO_FIELD(String, callback);
        DTO_FIELD(String, migrations);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //ENDER_LABEL_CONFIGDTO_HPP
