//
// Created by SURFACE on 2024/1/14.
//

#ifndef ENDER_LABEL_PAGEDREQUESTDTO_HPP
#define ENDER_LABEL_PAGEDREQUESTDTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)
namespace ender_label::dto {
    class PagedRequestDto : public oatpp::DTO {
        DTO_INIT(PagedRequestDto, oatpp::DTO)

        DTO_FIELD(Int32, page_size);

        DTO_FIELD(Int32, page_num);

    };
}
#include OATPP_CODEGEN_END(DTO)

#endif //ENDER_LABEL_PAGEDREQUESTDTO_HPP
