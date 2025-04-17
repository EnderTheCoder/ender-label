//
// Created by ender on 4/10/23.
//

#ifndef ENDER_LABEL_SWAGGERCOMPONENT_HPP
#define ENDER_LABEL_SWAGGERCOMPONENT_HPP

#include "oatpp-swagger/Model.hpp"
#include "oatpp-swagger/Resources.hpp"
#include <oatpp/core/macro/component.hpp>
namespace ender_label::component {
    class SwaggerComponent {
        /**
         *  General API docs info
         */
        OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::DocumentInfo>, swaggerDocumentInfo)([] {

            oatpp::swagger::DocumentInfo::Builder builder;

            builder
                    .setTitle("ender_label swagger ui service")
                    .setDescription("Another high performance C++ backend project powered by Oat++")
                    .setVersion(VERSION)
                    .setContactName("EnderTheCoder")
                    .setContactUrl("https://github.com/EnderTheCoder")
                    .setLicenseName("All rights reserved")
                    .addServer("http://49.209.30.134:4778/", "Test Server");

            return builder.build();

        }());


        /**
         *  Swagger-Ui Resources (<oatpp-examples>/lib/oatpp-swagger/res)
         */
        OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::Resources>, swaggerResources)([] {
            // Make sure to specify correct full path to oatpp-swagger/res folder !!!
            return oatpp::swagger::Resources::loadResources(OATPP_SWAGGER_RES_PATH);
        }());

    };
}

#endif //ENDER_LABEL_SWAGGERCOMPONENT_HPP
