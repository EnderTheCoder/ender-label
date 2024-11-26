//
// Created by ender on 11/26/24.
//

#ifndef POSEDTO_HPP
#define POSEDTO_HPP

#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)
namespace ender_label::dto::data::annotation {
    class PoseDto : public oatpp::DTO {
    private:
        class PointDto : public oatpp::DTO {
            DTO_INIT(PointDto, DTO)

            DTO_FIELD(Int32, cls_id);

            DTO_FIELD(Float64, normalized_x);

            DTO_FIELD(Float64, normalized_y);
        };

    public:
        DTO_INIT(PoseDto, DTO)

        DTO_FIELD(List<Object<PointDto>>, points);
    };
}
#include OATPP_CODEGEN_END(DTO)

#endif //POSEDTO_HPP
