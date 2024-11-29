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

        auto children(const bool direct = true) {
            if (direct) {
                return castWList<Permission>(toWrappedList(getByField("parent", getId())));
            }
            std::vector<std::shared_ptr<Permission> > res = {};
            auto children = this->children();
            if (!children.empty()) {
                res.insert(res.begin(), children.begin(), children.end());
                for (const auto &perm: children) {
                    if (const auto grand_children = perm->children(); !grand_children.empty())
                        res.insert(res.begin(), grand_children.begin(), grand_children.end()); {
                    }
                }
            }
            return res;
        }

        bool getIsChildOf(auto perm_id) {
            for (const auto perm = Permission::getById<Permission>(perm_id); const auto &_p: perm->children(false)) {
                if (_p->getId() == getId()) {
                    return true;
                }
            }
            return false;
        }

        bool getIsParentOf(auto perm_id) {
            for (const auto perm = Permission::getById<Permission>(perm_id); const auto &_p: this->children(false)) {
                if (_p->getId() == perm->getId()) {
                    return true;
                }
            }
            return false;
        }
    };
}


#endif //PERMISSION_HPP
