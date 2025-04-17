#ifndef ENDER_LABEL_COMMONNODATADTO_HPP_HPP
#define ENDER_LABEL_COMMONNODATADTO_HPP_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)
namespace ender_label::dto {
class BaseResponseDto : public oatpp::DTO {
    DTO_INIT(BaseResponseDto, DTO)

    DTO_FIELD(oatpp::Int16, code) = 100;

    DTO_FIELD_INFO(code) {
        info->description = "状态码,100代表成功";
    }
    DTO_FIELD(oatpp::String, message) = "请求成功";

    DTO_FIELD_INFO(message) {
        info->description = "返回值描述信息";
    }
};
}
#include OATPP_CODEGEN_END(DTO)

#endif //ENDER_LABEL_COMMONNODATADTO_HPP_HPP