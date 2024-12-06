//
// Created by ender on 11/26/24.
//

#ifndef PERMISSION_HPP
#define PERMISSION_HPP


#include "dto/data/PermissionDto.hpp"

namespace ender_label::service::user {
    constexpr char table_name_permission[] = "ender_label_permission";

    class Permission final : public ServiceBean<table_name_permission, data::PermissionDto> {
    public:
        static std::shared_ptr<Permission> fromKey(const std::string &key) {
            if (const auto objs = toWrappedList(getByField("key", String(key), true)); !objs.empty()) {
                return std::static_pointer_cast<Permission>(objs.front());
            }
            return nullptr;
        }

        static auto root() {
            return fromKey("ROOT");
        }

        auto parent() {
            return getById<Permission>(getDto()->parent_id);
        }

        auto updateParent(auto id) {
            const auto dto = data::PermissionDto::createShared();
            dto->parent_id = id;
            this->overwrite(dto);
        }

        auto uppers() {
            std::vector<std::shared_ptr<Permission> > res = {};
            for (std::shared_ptr<Permission> perm = parent(); perm != nullptr; perm = perm->parent()) {
                res.push_back(perm);
            }
            return res;
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

        bool getIsRoot() {
            return this->getDto()->key == "ROOT";
        }
    };
}


#endif //PERMISSION_HPP
