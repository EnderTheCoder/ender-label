//
// Created by ender on 11/26/24.
//

#ifndef SEGMENTATIONANNOTATIONDTO_HPP
#define SEGMENTATIONANNOTATIONDTO_HPP

#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::annotation {
    class SegmentationAnnotationDto : public oatpp::DTO {
    private:
        class PolygonDto : public oatpp::DTO {
            DTO_INIT(PolygonDto, DTO)

            DTO_FIELD(Int32, cls_id);

            // List[points...] -> List[x, y] -> Float32[0-1 float number]
            DTO_FIELD(List<List<Float32>>, normalized_points);
        };

        class MaskDto : public oatpp::DTO {
            DTO_INIT(MaskDto, DTO)

            DTO_FIELD(Int32, cls_id);

            // UnorderedSet[pixels] -> List[x, y] -> Int32[0 < val < (image's w or h)]
            DTO_FIELD(UnorderedSet<List<Int32>>, pixels);
        };

    public:
        DTO_INIT(SegmentationAnnotationDto, DTO)

        DTO_FIELD(List<Object<PolygonDto>>, polygons);

        DTO_FIELD(List<Object<MaskDto>>, masks);
    };
}

#include OATPP_CODEGEN_END(DTO)


#endif //SEGMENTATIONANNOTATIONDTO_HPP
