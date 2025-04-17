//
// Created by ender on 11/26/24.
//

#ifndef POSEDTO_HPP
#define POSEDTO_HPP


#include "ObjectDetectionDto.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include "ObjectDetectionDto.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::annotation {
    class PoseDto final : public oatpp::DTO {
    private:
        class PointDto final : public oatpp::DTO {
            DTO_INIT(PointDto, DTO)

            DTO_FIELD(Int32, cls_id);

            DTO_FIELD(Float64, normalized_x);

            DTO_FIELD(Float64, normalized_y);
        };

        class PoseBoxDto final : public ObjectDetectionDto::BoundingDto {
            DTO_INIT(PoseBoxDto, ObjectDetectionDto::BoundingDto)

            DTO_FIELD(List<Object<PointDto>>, points);
        };

    public:
        DTO_INIT(PoseDto, DTO)

        DTO_FIELD(List<Object<PoseBoxDto>>, bboxes);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //POSEDTO_HPP
