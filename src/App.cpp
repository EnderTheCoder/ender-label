//
// Created by ender on 23-7-23.
//

#include "oatpp/network/Server.hpp"

#include "ServiceComponent.hpp"
#include "DatabaseComponent.hpp"
#include "ClientComponent.hpp"
#include "SwaggerComponent.hpp"
#include "AppComponent.hpp"

#include "controller/DefaultController.hpp"
#include "oatpp-swagger/Controller.hpp"
#include "controller/DatasetController.hpp"
#include "controller/UserController.hpp"

#include "service/processor/BackgroundImageProcessor.hpp"

void run(const oatpp::base::CommandLineArguments &args) {
    ender_label::component::AppComponent appComponent(args);
    ender_label::component::DatabaseComponent databaseComponent;
    // ender_label::component::ClientComponent clientComponent;
    ender_label::component::ServiceComponent serviceComponent;
    ender_label::component::SwaggerComponent swaggerComponent;
    ender_label::service::processor::BackgroundImageProcessor::initThumbnailDir();
    auto img_processor = ender_label::service::processor::BackgroundImageProcessor::createShared();
    img_processor->start();
    OATPP_LOGI("Version", VERSION)
    OATPP_LOGD("Server", "Running on %s:%s",
               serviceComponent.serverConnectionProvider.getObject()->getProperty("host").toString()->c_str(),
               serviceComponent.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());

    auto router = serviceComponent.httpRouter.getObject();
    oatpp::web::server::api::Endpoints endpoints;
    // append more endpoints here
    endpoints.append(router->addController(ender_label::controller::DefaultController::createShared())->getEndpoints());
    endpoints.append(router->addController(ender_label::controller::UserController::createShared())->getEndpoints());
    endpoints.append(router->addController(ender_label::controller::DatasetController::createShared())->getEndpoints());

    // swagger is only enabled in dev env.
    const OATPP_COMPONENT(oatpp::Object<ender_label::dto::data::ConfigDto>, config);
    if (config->config_type == "development")
        router->addController(oatpp::swagger::Controller::createShared(endpoints));
    // start sync server
    oatpp::network::Server server(serviceComponent.serverConnectionProvider.getObject(),
                                  serviceComponent.serverConnectionHandler.getObject());
    server.run();
}

/**
 *  main
 */
int main(const int argc, const char *argv[]) {
    oatpp::base::Environment::init();
    run(oatpp::base::CommandLineArguments(argc, argv));
    oatpp::base::Environment::destroy();
    return 0;
}
