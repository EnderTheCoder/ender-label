//
// Created by ender on 22-8-26.
//

#ifndef ENDER_LABEL_SERVICECOMPONENT_HPP
#define ENDER_LABEL_SERVICECOMPONENT_HPP

#include "dto/data/ConfigDto.hpp"
#include "ErrorHandler.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/interceptor/AllowCorsGlobal.hpp"
// #include "service/img/BackgroundImageProcessor.hpp"
namespace ender_label::component {
    class ServiceComponent {
    private:
        typedef oatpp::web::protocol::http::Status Status;
    public:



        /**
         *  Create ConnectionProvider component which listens on the port
         */
        OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] {
            const OATPP_COMPONENT(oatpp::Object<dto::data::ConfigDto>, configComponent); // Get config component
            return oatpp::network::tcp::server::ConnectionProvider::createShared(
                    {configComponent->host, configComponent->port, oatpp::network::Address::UNSPEC});
        }());

        /**
         *  Create Router component
         */
        OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
            return oatpp::web::server::HttpRouter::createShared();
        }());

        /**
         *  Create ConnectionHandler component which uses Router component to route requests
         */
        OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {
            OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component
            OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>,
                            objectMapper); // get ObjectMapper component

            auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);
            connectionHandler->addRequestInterceptor(
                    std::make_shared<oatpp::web::server::interceptor::AllowOptionsGlobal>());
            connectionHandler->addResponseInterceptor(std::make_shared<oatpp::web::server::interceptor::AllowCorsGlobal>());
            connectionHandler->setErrorHandler(std::make_shared<ErrorHandler>(objectMapper));
            return connectionHandler;
        }());
    };
}
#endif
