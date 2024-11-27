//
// Created by ender on 11/26/24.
//

#ifndef USERCONTROLLER_HPP
#define USERCONTROLLER_HPP
#include "service/user/Permission.hpp"
#include "service/user/User.hpp"
#include "dto/request/LoginRequestDto.hpp"
#include "dto/response/SimpleDataResponseDto.hpp"
#include "util/ControllerUtil.hpp"
#include "util/AuthUtil.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

namespace ender_label::controller {
    using namespace service;

    class UserController : public oatpp::web::server::api::ApiController {
    private:
        explicit UserController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper) {
            this->setErrorHandler(std::make_shared<component::ErrorHandler>(objectMapper));
        }

    public:
        static std::shared_ptr<UserController>
        createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
            return std::shared_ptr<UserController>(new UserController(objectMapper));
        }

    public:

        ENDPOINT("GET", "/user/login", login, BODY_DTO(Object<request::LoginRequestDto>, req)) {
            REQUEST_ALL_PARAM_CHECK(req)
            const auto resp = SimpleDataResponseDto<Object<data::UserDto>>::createShared();
            const auto users = user::User::toWrappedList(user::User::getByField("username", req->username, true));
            if (users.empty() or users.front()->getDto()->password != req->password) {
                resp->code = -100;
                resp->data = "Wrong username or password";
                return createDtoResponse(Status::CODE_200,resp);
            }
            resp->data = users.front()->getDto();
            return createDtoResponse(Status::CODE_200,resp);
        }

        ENDPOINT("GET", "/user/register", _register) {

        }

        ENDPOINT("GET", "/user/permission/ch", chPermission, AUTH_HEADER) {
            AUTH(authorization)
            if (!USER->hasPerm("ROOT")) {
                ERROR(Status::CODE_403, "Permission denied.")
            }



        }

        ENDPOINT("GET", "/user/add", addUser) {
        }

        ENDPOINT("GET", "/user/rm", rmUser) {
        }

        ENDPOINT("GET", "/user/info", getUser) {
        }
    };
}

#include OATPP_CODEGEN_END(ApiController)

#endif //USERCONTROLLER_HPP
