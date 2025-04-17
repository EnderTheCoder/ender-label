//
// Created by ender on 24-1-19.
//

#ifndef ENDER_LABEL_COUNTDTO_HPP
#define ENDER_LABEL_COUNTDTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)
namespace ender_label::dto {
    template<typename INT_TYPE>
    class CountDto final : public oatpp::DTO {
        DTO_INIT(CountDto, DTO)

        DTO_FIELD(INT_TYPE, count);
    };
}
#include OATPP_CODEGEN_END(DTO)


#endif //ENDER_LABEL_COUNTDTO_HPP
