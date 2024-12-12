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
        std::unordered_map<std::string, std::string> asset_ext_white_list = {
            {".jpg", "image/jpg"},
            {".png", "image/png"},
            {".css", "text/css"},
            {".js", "application/javascript"}
        };

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
        ENDPOINT("GET", "/", index) {
            const OATPP_COMPONENT(Object<data::ConfigDto>, config);
            const auto index_path = config->static_root->c_str() / std::filesystem::path("index.html");
            return createResponse(Status::CODE_200, String::loadFromFile(index_path.c_str()));
        }

        ENDPOINT("GET", "/assets/{asset_file}", asset, PATH(String, asset_file)) {
            const OATPP_COMPONENT(Object<data::ConfigDto>, config);
            const auto asset_path = std::filesystem::path(config->static_root->c_str()) / "assets" / asset_file->
                                    c_str();
            OATPP_ASSERT_HTTP(asset_ext_white_list.contains(asset_path.extension().string()), Status::CODE_403,
                              "Rejected file extension.")
            const auto resp = createResponse(Status::CODE_200, String::loadFromFile(asset_path.c_str()));
            resp->putHeader("Content-Type", asset_ext_white_list.at(asset_path.extension().string()));
            return resp;
        }
    };
}


#include OATPP_CODEGEN_END(ApiController)

#endif
