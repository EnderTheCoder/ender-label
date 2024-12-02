//
// Created by ender on 11/26/24.
//

#ifndef PERMISSIONDTO_HPP
#define PERMISSIONDTO_HPP


#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data {
    class PermissionDto : public oatpp::DTO {
        DTO_INIT(PermissionDto, DTO)

        DTO_FIELD(Int32, id);

        DTO_FIELD(String, key);

        DTO_FIELD(Int32, parent_id);

        DTO_FIELD(String, desc);
    };
}

#include OATPP_CODEGEN_END(DTO)


#endif //PERMISSIONDTO_HPP
