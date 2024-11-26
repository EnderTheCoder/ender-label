
#ifndef ENDER_LABEL_ERRORHANDLER_HPP
#define ENDER_LABEL_ERRORHANDLER_HPP

#include "dto/response/StatusDto.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"
#include "dto/data/ConfigDto.hpp"
#include "oatpp/core/macro/component.hpp"
namespace ender_label::component {
    class ErrorHandler final : public oatpp::web::server::handler::ErrorHandler {
    private:
        typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
        typedef oatpp::web::protocol::http::Status Status;
        typedef oatpp::web::protocol::http::outgoing::ResponseFactory ResponseFactory;
    private:
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
    public:

        explicit ErrorHandler(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
                : m_objectMapper(std::move(objectMapper)) {}

        std::shared_ptr<OutgoingResponse>
        handleError(const Status &status, const oatpp::String &message, const Headers &headers) override {
            const OATPP_COMPONENT(oatpp::Object<dto::data::ConfigDto>, config);
            const auto error = dto::StatusDto::createShared();
            error->status = "ERROR";
            error->code = status.code;
            error->message = message;
            auto response = ResponseFactory::createResponse(status, error, m_objectMapper);
            for (const auto &[key, val]: headers.getAll()) {
                response->putHeader(key.toString(), val.toString());
            }
            if (error->code >= 500 && error->code < 600) {
                OATPP_LOGE("CODE-" + std::to_string(error->code), error->message->c_str())
            } else {
                OATPP_LOGW("CODE-" + std::to_string(error->code), error->message->c_str())
            }

            if (error->code == 500 && config->config_type == "production") {
                error->message = "服务器发生内部错误";
            }
            return response;

        }

    };
}

#endif
