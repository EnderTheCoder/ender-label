//
// Created by ender on 22-9-29.
//
#include <oatpp/web/server/api/ApiController.hpp>
#include "util/ConceptUtil.hpp"
#ifndef ENDER_LABEL_CONTROLLERUTIL_HPP
#define ENDER_LABEL_CONTROLLERUTIL_HPP

#define REQUEST_PARAM_CHECK(PARAM) \
if(PARAM == nullptr) { throw oatpp::web::protocol::http::HttpError(oatpp::web::protocol::http::Status::CODE_400, "不完整的参数"); }

#define REQUEST_ALL_PARAM_CHECK(REQUEST_DTO) \
if(!ender_label::util::ControllerUtil::isAllParamNoNull(REQUEST_DTO)) { throw oatpp::web::protocol::http::HttpError(oatpp::web::protocol::http::Status::CODE_400, "不完整的参数"); }


namespace ender_label::util {
    class ControllerUtil {
    private:
        typedef oatpp::web::protocol::http::Status Status;

    public:
        template<DerivedFromDTO T>
        static auto isAllParamNoNull(const oatpp::Object<T> &dto) {
            for (auto &pair: oatpp::Object<T>::getPropertiesMap()) {
                if (pair.second->get(dto.get()) == nullptr) {
                    return false;
                }
            }
            return true;
        }
    };
}


#endif //ENDER_LABEL_CONTROLLERUTIL_HPP
