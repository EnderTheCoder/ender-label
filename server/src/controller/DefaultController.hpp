//
// Created by ender on 22-10-9.
//

#ifndef ENDER_LABEL_DEFAULTCONTROLLER_HPP
#define ENDER_LABEL_DEFAULTCONTROLLER_HPP

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/core/macro/component.hpp>
#include "ErrorHandler.hpp"
#include "iostream"
#include "service/ServiceBean.hpp"
#include "dto/request/ExportDatasetRequestDto.hpp"
#include OATPP_CODEGEN_BEGIN(ApiController)
namespace ender_label::controller {
    using namespace service;
    class DefaultController : public oatpp::web::server::api::ApiController {
    private:
        explicit DefaultController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
                : oatpp::web::server::api::ApiController(objectMapper) {
            this->setErrorHandler(std::make_shared<component::ErrorHandler>(objectMapper));
        }
    public:
        static std::shared_ptr<DefaultController>
        createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
            return std::shared_ptr<DefaultController>(new DefaultController(objectMapper));
        }

    public:
        ENDPOINT("GET", "/", hello) {
            return this->createResponse(Status::CODE_200, "<h1>Welcome</h1><p>Just another powerful backend powered by Oat++</p>");
        }
    };
}

#include OATPP_CODEGEN_END(ApiController)

#endif
