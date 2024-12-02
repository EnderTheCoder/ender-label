//
// Created by ender on 23-7-23.
//

#ifndef ENDER_LABEL_APPCOMPONENT_HPP
#define ENDER_LABEL_APPCOMPONENT_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp-postgresql/orm.hpp"
#include "dto/data/ConfigDto.hpp"
#include "oatpp/core/base/CommandLineArguments.hpp"

namespace ender_label::component {
    using namespace dto;

    class AppComponent {
    private:
        oatpp::base::CommandLineArguments m_cmdArgs;

    public:
        explicit AppComponent(const oatpp::base::CommandLineArguments &cmdArgs)
            : m_cmdArgs(cmdArgs) {
        }

    public:
        /**
       * This should be configured through config-server ex. Consul
       */
        OATPP_CREATE_COMPONENT(oatpp::Object<data::ConfigDto>, config)([this] {
            const auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

            const char *config_path = std::getenv("CONFIG_PATH"); // first read from env variable
            if (config_path == nullptr) {
                config_path = m_cmdArgs.getNamedArgumentValue("--config",
                                                              "../resources/config.json");
                // if no env variable get from command line
            }

            if (const oatpp::String configText = oatpp::String::loadFromFile(config_path)) {
                const auto profiles = objectMapper->readFromString<oatpp::Fields<oatpp::Object<data::ConfigDto> > >(
                    configText);

                const char *profileArg = std::getenv("CONFIG_PROFILE"); // first read from env variable
                if (profileArg == nullptr) {
                    profileArg = m_cmdArgs.getNamedArgumentValue("--profile",
                                                                 "development@default");
                    // if no env variable get from command line
                }

                OATPP_LOGD("Server", "Loading configuration profile '%s'", profileArg)

                auto profile = profiles.getValueByKey(profileArg, nullptr);
                if (!profile) {
                    throw std::runtime_error("No configuration profile found. Server won't run.");
                }
                std::vector<std::string> str;

                boost::split(str, profileArg, boost::is_any_of("@"));
                if (str.size() != 2 || (str.at(0) != "development" && str.at(0) != "production")) {
                    throw std::runtime_error("Wrong config profile indicator format.");
                }

                profile->config_type = str.at(0);

                const auto storage_root = boost::filesystem::path(profile->storage);
                if (not(exists(storage_root) and is_directory(storage_root))) {
                    throw std::runtime_error("Invalid storage path: not a directory or not exist.");
                }
                auto sub_dirs = std::array{"datasets"};
                std::ranges::for_each(sub_dirs, [&storage_root](auto &x) {
                    if (not exists(storage_root / x)) boost::filesystem::create_directory(storage_root / x);
                });
                return profile;
            }

            OATPP_LOGE("AppComponent", "Can't load configuration file at path '%s'", config_path)
            throw std::runtime_error("[AppComponent]: Can't load configuration file");
        }());
        /**
         *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
         */
        OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] {
            auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
            mapper->getSerializer()->getConfig()->useBeautifier = false;
            mapper->getSerializer()->getConfig()->includeNullFields = false;
            mapper->getSerializer()->getConfig()->escapeFlags = 0;
            return mapper;
        }());
    };
}
#endif //ENDER_LABEL_APPCOMPONENT_HPP
