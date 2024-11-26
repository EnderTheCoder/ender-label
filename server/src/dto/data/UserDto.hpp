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
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //USERDTO_HPP
