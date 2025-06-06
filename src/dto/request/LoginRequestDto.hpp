#ifndef LOGIN_REQUEST_DTO_HPP
#define LOGIN_REQUEST_DTO_HPP
#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::request {
    class LoginRequestDto : public oatpp::DTO {
        DTO_INIT(LoginRequestDto, DTO)
        DTO_FIELD(String, username);
        DTO_FIELD_INFO(username) {
            info->required = true;
            info->description = "用户名";
        }

        DTO_FIELD(String, password);
        DTO_FIELD_INFO(password) {
            info->required = true;
            info->description = "密码";
        }
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //LOGIN_REQUEST_DTO_HPP
