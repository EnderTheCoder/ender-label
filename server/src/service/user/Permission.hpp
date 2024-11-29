//
// Created by ender on 11/26/24.
//

#ifndef PERMISSION_HPP
#define PERMISSION_HPP


#include "dto/data/PermissionDto.hpp"

namespace ender_label::service::user {
    constexpr char table_name_permission[] = "permission";

    class Permission final : public ServiceBean<table_name_permission, data::PermissionDto> {
    public:
        auto parent() {
            return getById(getDto()->parent);
        }

        auto children() {
            return toWrappedList(getByField("parent", getId()));
        }

        auto get_is_child_of(auto perm_id) {
        }

        auto get_is_parent_of(auto perm_id) {
        }
    };
}


#endif //PERMISSION_HPP
