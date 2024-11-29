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

    class UserController final : public oatpp::web::server::api::ApiController {
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
                resp->message = "Wrong username or password";
                return createDtoResponse(Status::CODE_200,resp);
            }
            resp->data = users.front()->getDto();
            return createDtoResponse(Status::CODE_200,resp);
        }

        ENDPOINT("GET", "/user/register", _register) {

        }

        ENDPOINT("GET", "/user/permission/ch", chPermission, AUTH_HEADER, BODY_DTO(UnorderedSet<Int32>, perms)) {
            AUTH
            if (!USER->hasPerm("ROOT")) {
                ERROR(Status::CODE_403, "Permission denied.")
            }
            const auto dto = data::UserDto::createShared();
            USER->overwrite(dto);
            return createDtoResponse(Status::CODE_200, BaseResponseDto::createShared());
        }

        ENDPOINT("GET", "/user/add", addUser, AUTH_HEADER, BODY_DTO(Object<data::UserDto>, user_dto)) {
            AUTH
            REQUEST_PARAM_CHECK(user_dto->email)
            REQUEST_PARAM_CHECK(user_dto->password)
            REQUEST_PARAM_CHECK(user_dto->permission_ids)
            REQUEST_PARAM_CHECK(user_dto->username)
            if (!USER->hasPerm("ROOT")) {
                ERROR(Status::CODE_403, "Permission denied.")
            }
            const auto user = user::User::createShared(user_dto);
            user->write();
            return createDtoResponse(Status::CODE_200, BaseResponseDto::createShared());
        }

        ENDPOINT("GET", "/user/rm", rmUser, AUTH_HEADER) {
            AUTH
            if (!USER->hasPerm("ROOT")) {
                ERROR(Status::CODE_403, "Permission denied.")
            }
            USER->del();
            return createDtoResponse(Status::CODE_200, BaseResponseDto::createShared());
        }

        ENDPOINT("GET", "/user/info", getUser, AUTH_HEADER) {
            AUTH
            const auto resp = SimpleDataResponseDto<Object<data::UserDto>>::createShared();
            resp->data = USER->getDto();
            return createDtoResponse(Status::CODE_200, resp);
        }
    };
}

#include OATPP_CODEGEN_END(ApiController)

#endif //USERCONTROLLER_HPP
