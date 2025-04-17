//
// Created by ender on 11/26/24.
//

#ifndef PERMISSIONDTO_HPP
#define PERMISSIONDTO_HPP


#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data {
    class PermissionDto : public oatpp::DTO {
        DTO_INIT(PermissionDto, DTO)

        DTO_FIELD(Int32, id);

        DTO_FIELD(String, key);

        DTO_FIELD_INFO(key) {
            info->description = "权限键，大写下划线连接，不可重复";
            info->required = true;
        }

        DTO_FIELD(Int32, parent_id);

        DTO_FIELD_INFO(parent_id) {
            info->description = "上一级权限节点的id";
            info->required = true;
        }

        DTO_FIELD(String, desc);

        DTO_FIELD_INFO(desc) {
            info->description = "权限描述";
        }
    };
}

#include OATPP_CODEGEN_END(DTO)


#endif //PERMISSIONDTO_HPP
