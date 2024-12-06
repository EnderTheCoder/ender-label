//
// Created by ender on 11/27/24.
//

#ifndef AUTHUTIL_HPP
#define AUTHUTIL_HPP
#include "service/user/User.hpp"
#include "oatpp/codegen/ApiController_define.hpp"
#define AUTH \
    auto _user = ender_label::util::get_current_user(_authorization); \
    if (_user == nullptr) {throw web::protocol::http::HttpError(oatpp::web::protocol::http::Status::CODE_403, "Permission denied."); }

#define USER _user

#define AUTH_HEADER HEADER(String, _authorization, "Authorization")

namespace ender_label::util {
    std::shared_ptr<service::user::User> get_current_user(auto sess) {
        using namespace ender_label::service::user;
        auto users = User::toWrappedList(User::getByField("token", sess));
        if (users.empty()) {
            return nullptr;
        }
        return std::static_pointer_cast<User>(users.front());
    }
}
#endif //AUTHUTIL_HPP
