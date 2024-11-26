//
// Created by ender on 11/26/24.
//

#ifndef ANNOTATIONCLASSDTO_HPP
#define ANNOTATIONCLASSDTO_HPP
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::annotation {
    class AnnotationClassDto : public oatpp::DTO {
        DTO_INIT(AnnotationClassDto, DTO)
        DTO_FIELD(Int32, id);
        DTO_FIELD(String, key); // english words only
        DTO_FIELD(String, desc);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //ANNOTATIONCLASSDTO_HPP
