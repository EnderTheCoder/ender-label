#ifndef LABELME_SEGMENTATION_DTO_HPP
#define LABELME_SEGMENTATION_DTO_HPP
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::annotation::foreign {
    class LabelmeSegmentationDto final : public oatpp::DTO {
    public:
        class ShapeDto final : public oatpp::DTO {
            DTO_INIT(ShapeDto, DTO)

            DTO_FIELD(String, label);

            DTO_FIELD(List<List<Float64>>, points);

            DTO_FIELD(Int32, group_id);

            DTO_FIELD(String, description);

            DTO_FIELD(String, shape_type);

            DTO_FIELD(UnorderedFields<Void>, flags);

            DTO_FIELD(Void, mask);
        };

        DTO_INIT(LabelmeSegmentationDto, DTO)

        DTO_FIELD(String, version);

        DTO_FIELD(UnorderedFields<Void>, flags);

        DTO_FIELD(List<ShapeDto>, shapes);

        DTO_FIELD(String, imagePath);

        DTO_FIELD(String, imageData);

        DTO_FIELD(String, imageHeight);

        DTO_FIELD(String, imageWidth);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //LABELME_SEGMENTATION_DTO_HPP
