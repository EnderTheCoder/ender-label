#ifndef EXPORT_DATASET_REQUEST_DTO_HPP
#define EXPORT_DATASET_REQUEST_DTO_HPP
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::request {
    class ExportDatasetRequestDto : public oatpp::DTO {
        DTO_INIT(ExportDatasetRequestDto, DTO)

        DTO_FIELD(String, type);

        DTO_FIELD(UnorderedFields<Int32>, img_to_anno); // img_to_anno: ["img_name":{anno_id_1, anno_id_2,anno_id_n}]


    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //EXPORT_DATASET_REQUEST_DTO_HPP
