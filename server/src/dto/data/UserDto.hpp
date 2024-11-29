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

        DTO_FIELD(String, email);

        DTO_FIELD(String, password);

        DTO_FIELD(String, session);

        DTO_FIELD(UnorderedSet<Int32>, permission_ids);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //USERDTO_HPP
