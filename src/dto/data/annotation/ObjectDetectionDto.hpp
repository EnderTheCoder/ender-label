//
// Created by ender on 11/26/24.
//

#ifndef BBOXDTO_HPP
#define BBOXDTO_HPP


#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::annotation {
    class ObjectDetectionDto : public oatpp::DTO {
        public:
        class BoundingDto : public oatpp::DTO {
        public:
            DTO_INIT(BoundingDto, DTO)

            DTO_FIELD(Int32, cls_id);

            DTO_FIELD(List<Float64>, normalized_xyxy);
        };

        DTO_INIT(ObjectDetectionDto, DTO)

        DTO_FIELD(List<Object<BoundingDto>>, bboxes);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //BBOXDTO_HPP
