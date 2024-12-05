#ifndef REGISTER_REQUEST_DTO_HPP
#define REGISTER_REQUEST_DTO_HPP
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::request {
    class RegisterRequestDto final : public oatpp::DTO {
        DTO_INIT(RegisterRequestDto, DTO)
        DTO_FIELD(String, username);
        DTO_FIELD(String, password);
        DTO_FIELD(String, email);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //REGISTER_REQUEST_DTO_HPP
