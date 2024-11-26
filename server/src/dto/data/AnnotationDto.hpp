//
// Created by ender on 11/26/24.
//

#ifndef ANNOTATIONDTO_HPP
#define ANNOTATIONDTO_HPP

#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data {
    class AnnotationDto : public oatpp::DTO {
        DTO_INIT(AnnotationDto, DTO)
        DTO_FIELD(Int32, id);

        DTO_FIELD(Int32, owner_id);

        DTO_FIELD(String, img_name);

        DTO_FIELD(String, type);

        DTO_FIELD(String, raw_json);

        DTO_FIELD(Int32, width);

        DTO_FIELD(Int32, height);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //ANNOTATIONDTO_HPP
