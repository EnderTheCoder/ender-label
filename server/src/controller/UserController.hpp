//
// Created by ender on 11/26/24.
//

#ifndef USERCONTROLLER_HPP
#define USERCONTROLLER_HPP
#include "service/user/Permission.hpp"
#include "service/user/User.hpp"
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
        ENDPOINT("GET", "/user/permission/ch", chPermission) {
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
