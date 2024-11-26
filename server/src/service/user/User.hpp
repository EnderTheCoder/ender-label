//
// Created by ender on 11/26/24.
//

#ifndef USER_HPP
#define USER_HPP

#include "dto/data/UserDto.hpp"

namespace ender_label::service::user {
    constexpr char table_name_user[] = "user";

    class User : public ServiceBean<table_name_user, data::UserDto> {
    };
}

#endif //USER_HPP
