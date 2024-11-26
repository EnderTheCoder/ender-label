#ifndef ENDER_LABEL_COMMONPAGEDRESPONSEDTO_HPP_HPP
#define ENDER_LABEL_COMMONPAGEDRESPONSEDTO_HPP_HPP

#include "SimpleDataResponseDto.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)
namespace ender_label::dto {

template<typename T>
class PagedResponseDto : public SimpleDataResponseDto<oatpp::Vector<T>> {
    DTO_INIT(PagedResponseDto, SimpleDataResponseDto<oatpp::Vector<T>>)

    DTO_FIELD(oatpp::Int32, page_num);

    DTO_FIELD(oatpp::Int32, page_size);

    DTO_FIELD(oatpp::UInt64, page_total);
};
}
#include OATPP_CODEGEN_END(DTO)

#endif //ENDER_LABEL_COMMONPAGEDRESPONSEDTO_HPP_HPP