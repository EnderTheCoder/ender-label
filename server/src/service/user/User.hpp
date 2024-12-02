//
// Created by ender on 11/26/24.
//

#ifndef USER_HPP
#define USER_HPP

#include "dto/data/UserDto.hpp"
#include  "util/TimeUtil.hpp"
#include  "util/Util.hpp"
#include "Permission.hpp"

namespace ender_label::service::user {
    constexpr char table_name_user[] = "ender_label_user";

    class User final : public ServiceBean<table_name_user, data::UserDto> {
    public:
        bool hasPerm(const std::string &perm_key) {
            const auto perms = Permission::toDtoList(Permission::getByField("key", String(perm_key)));
            return this->hasPerm(perms->front()->id);
        }

        bool hasPerm(const int perm_id) {
            if (this->getDto()->permission_ids->contains(perm_id)) return true;
            if (const auto perm = Permission::getById<Permission>(perm_id); std::ranges::any_of(
                perm->uppers(), [this](auto &_p) {
                    return this->getDto()->permission_ids->contains(_p->getDto()->id);
                }))
                return true;
            return false;
        }

        void rmPerm(const std::string &perm_key) {
            const auto perms = Permission::toDtoList(Permission::getByField("key", String(perm_key)));
            this->rmPerm(perms->front()->id);
        }

        void rmPerm(const int &perm_id) {
            const auto dto = data::UserDto::createShared();
            dto->permission_ids = this->getDto()->permission_ids;
            for (const auto perm = std::static_pointer_cast<Permission>(Permission::getById(perm_id));
                 const auto &child: perm->children(false)) {
                dto->permission_ids->erase(child->getId());
            }
            dto->permission_ids->erase(perm_id);
            this->overwrite(dto);
        }

        void addPerm(const std::string &perm_key) {
            const auto perms = Permission::toDtoList(Permission::getByField("key", String(perm_key)));
            this->addPerm(perms->front()->id);
        }

        void addPerm(const int &perm_id) {
            const auto dto = data::UserDto::createShared();
            dto->permission_ids = this->getDto()->permission_ids;
            dto->permission_ids->emplace(perm_id);
            this->overwrite(dto);
        }

        bool getIsSessionValid(const std::string &session) {
            return this->getDto()->session != nullptr and this->getDto()->session == session;
        }

        std::string refreshSession() {
            const auto origin = std::to_string(util::TimeUtil::getCurrentTimestampInLong()) + this->getDto()->username +
                                "EnderTheCoder";
            auto result = std::string();
            util::Util::computeHash(origin, result);
            const auto dto = data::UserDto::createShared();
            dto->session = result;
            this->overwrite(dto);
            return result;
        }
    };
}

#endif //USER_HPP
