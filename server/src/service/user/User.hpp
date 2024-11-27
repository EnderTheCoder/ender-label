//
// Created by ender on 11/26/24.
//

#ifndef USER_HPP
#define USER_HPP

#include "dto/data/UserDto.hpp"
#include  "util/TimeUtil.hpp"
#include  "util/Util.hpp"

namespace ender_label::service::user {
    constexpr char table_name_user[] = "user";

    class User : public ServiceBean<table_name_user, data::UserDto> {
    public:
        bool hasPerm(const std::string &perm_key) {
            const auto perms = Permission::toDtoList(Permission::getByField("key", String(perm_key)));
            return this->getDto()->permission_ids->contains(perms->front()->id);
        }

        bool hasPerm(const int perm_id) {
            return this->getDto()->permission_ids->contains(perm_id);
        }

        bool get_is_session_valid(const std::string &session) {
            return this->getDto()->session != nullptr and this->getDto()->session == session;
        }

        std::string refresh_session() {
            const auto origin = std::to_string(util::TimeUtil::getCurrentTimestampInLong()) + this->getDto()->username + "EnderTheCoder";
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
