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

        auto children(bool direct = true) {
            return toWrappedList(getByField("parent", getId()));
        }

        bool getIsChildOf(auto perm_id) {
        }

        bool getIsParentOf(auto perm_id) {
        }
    };
}


#endif //PERMISSION_HPP
