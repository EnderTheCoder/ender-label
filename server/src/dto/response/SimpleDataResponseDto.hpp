#ifndef ENDER_LABEL_COMMONSIMPLEDATADTO_HPP_HPP
#define ENDER_LABEL_COMMONSIMPLEDATADTO_HPP_HPP

#include "BaseResponseDto.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)
namespace ender_label::dto {

template<typename T>
class SimpleDataResponseDto : public BaseResponseDto {
    DTO_INIT(SimpleDataResponseDto, BaseResponseDto)

    DTO_FIELD(T, data);
};
}
#include OATPP_CODEGEN_END(DTO)

#endif //ENDER_LABEL_COMMONSIMPLEDATADTO_HPP_HPP