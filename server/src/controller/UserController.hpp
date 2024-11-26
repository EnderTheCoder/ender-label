//
// Created by ender on 11/26/24.
//

#ifndef USERCONTROLLER_HPP
#define USERCONTROLLER_HPP
#include "service/user/Permission.hpp"
#include "service/user/User.hpp"
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
    };
}

#endif //USERCONTROLLER_HPP
