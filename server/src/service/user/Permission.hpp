//
// Created by ender on 11/26/24.
//

#ifndef PERMISSION_HPP
#define PERMISSION_HPP


#include "dto/data/PermissionDto.hpp"

namespace ender_label::service::user {
    constexpr char table_name_permission[] = "permission";

    class Permission final : public ServiceBean<table_name_permission, data::PermissionDto> {
        auto parent() {
            return getById(this->getDto()->parent);
        }

        auto children() {
            return toWrappedList(getByField("parent", getId()));
        }
    };
}


#endif //PERMISSION_HPP
