#ifndef ENDER_LABEL_COMMONSEARCHRESULTDTO_HPP_HPP
#define ENDER_LABEL_COMMONSEARCHRESULTDTO_HPP_HPP

#include "SimpleDataResponseDto.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)
namespace ender_label::dto {
template<typename T>
class ArrayResponseDto : public SimpleDataResponseDto<oatpp::Vector<T>> {
    DTO_INIT(ArrayResponseDto, SimpleDataResponseDto<oatpp::Vector<T>>)

    DTO_FIELD(oatpp::UInt64, data_count);
};
}
#include OATPP_CODEGEN_END(DTO)

#endif //ENDER_LABEL_COMMONSEARCHRESULTDTO_HPP_HPP