#ifndef ANNOTATION_STATISTIC_DTO_HPP
#define ANNOTATION_STATISTIC_DTO_HPP
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::annotation {
    class AnnotationStatisticDto : public oatpp::DTO {
        DTO_INIT(AnnotationStatisticDto, DTO)
        DTO_FIELD(Int32, id);

//        DTO_FIELD(String,);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //ANNOTATION_STATISTIC_DTO_HPP