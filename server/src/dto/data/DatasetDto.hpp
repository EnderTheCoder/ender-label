//
// Created by ender on 11/26/24.
//

#ifndef DATASETDTO_HPP
#define DATASETDTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)
namespace ender_label::dto::data {
    class DatasetDto : public oatpp::DTO {
        DTO_INIT(DatasetDto, DTO)
        DTO_FIELD(Int32, id);
        DTO_FIELD(Int32, owner_id);
        DTO_FIELD(String, name);
        DTO_FIELD(String, desc);
        DTO_FIELD(UnorderedSet<String>, img_files);
        DTO_FIELD(UnorderedSet<Int32>, annotation_ids);
    };
}
#include OATPP_CODEGEN_END(DTO)


#endif //DATASETDTO_HPP
