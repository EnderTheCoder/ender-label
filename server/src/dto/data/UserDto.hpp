//
// Created by ender on 11/26/24.
//

#ifndef USERDTO_HPP
#define USERDTO_HPP


#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data {
    class UserDto : public oatpp::DTO {
        DTO_INIT(UserDto, DTO)

        DTO_FIELD(Int32, id);

        DTO_FIELD(String, username);
        DTO_FIELD_INFO(username) {
            info->description = "用户名";
            info->required = true;
        }

        DTO_FIELD(String, email);
        DTO_FIELD_INFO(email) {
            info->description = "邮箱";
            info->required = true;
        }

        DTO_FIELD(String, password);
        DTO_FIELD_INFO(password) {
            info->description = "用户的密码";
            info->required = true;
        }

        DTO_FIELD(String, session);
        DTO_FIELD_INFO(session) {
            info->description = "用户的会话（32位随机字符串）";
        }

        DTO_FIELD(UnorderedSet<Int32>, permission_ids);
        DTO_FIELD_INFO(permission_ids) {
            info->description = "用户的权限节点id列表";
        }
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //USERDTO_HPP
