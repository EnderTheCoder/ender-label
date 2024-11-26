//
// Created by ender on 11/26/24.
//

#ifndef POSEDTO_HPP
#define POSEDTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class PoseDto : public oatpp::DTO {
private:
    class PointDto : public oatpp::DTO {
        DTO_INIT(PointDto, DTO)

        DTO_FIELD(Float64, x);
        DTO_FIELD(Float64, y);
    };

public:
    DTO_INIT(PoseDto, DTO)

    DTO_FIELD(List<Object<PointDto>>, points);
};

#include OATPP_CODEGEN_END(DTO)

#endif //POSEDTO_HPP
