//
// Created by ender on 11/26/24.
//

#ifndef ANNOTATIONCLASSDTO_HPP
#define ANNOTATIONCLASSDTO_HPP
#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::annotation {
    class AnnotationClassDto : public oatpp::DTO {
        DTO_INIT(AnnotationClassDto, DTO)
        DTO_FIELD(Int32, id);

        DTO_FIELD(String, key); // english words only
        DTO_FIELD_INFO(key) {
            info->description = "用于描述该类别的一个简短的英文单词，可以用多个单词和下划线构成一个key";
            info->required = true;
        }

        DTO_FIELD(String, desc);
        DTO_FIELD_INFO(desc) {
            info->description = "对于该类别的详细描述，可以为空";
        }
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //ANNOTATIONCLASSDTO_HPP
