//
// Created by ender on 11/26/24.
//

#ifndef SEGMENTATIONANNOTATIONDTO_HPP
#define SEGMENTATIONANNOTATIONDTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::annotation {
    class SegmentationDto final : public oatpp::DTO {
    public:
        class PolygonDto final : public oatpp::DTO {
            DTO_INIT(PolygonDto, DTO)

            DTO_FIELD(Int32, cls_id);

            // List[points...] -> List[x, y] -> Float32[0-1 float number]
            DTO_FIELD(List<List<Float64>>, normalized_points) = {};
        };

        class MaskDto final : public oatpp::DTO {
            DTO_INIT(MaskDto, DTO)

            DTO_FIELD(Int32, cls_id);

            // List[pixels] -> List[x, y] -> Int32[0 < val < (image's w or h)]
            DTO_FIELD(List<List<Int32>>, pixels) = {};
        };

        DTO_INIT(SegmentationDto, DTO)

        DTO_FIELD(List<Object<PolygonDto>>, polygons) = {};

        DTO_FIELD(List<Object<MaskDto>>, masks) = {};
    };
}

#include OATPP_CODEGEN_END(DTO)


#endif //SEGMENTATIONANNOTATIONDTO_HPP
