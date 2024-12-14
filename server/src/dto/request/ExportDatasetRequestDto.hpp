#ifndef EXPORT_DATASET_REQUEST_DTO_HPP
#define EXPORT_DATASET_REQUEST_DTO_HPP
#include <dto/data/annotation/AnnotationDto.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::request {
    class ExportDatasetRequestDto : public oatpp::DTO {
        DTO_INIT(ExportDatasetRequestDto, DTO)

        DTO_FIELD(Enum<TaskType>::AsString, task_type);

        DTO_FIELD_INFO(task_type) {
            info->description = "图片标注任务类型";
        }

        DTO_FIELD(Boolean, annotated_only);

        DTO_FIELD_INFO(annotated_only) {
            info->description = "是否只导出已经标注的数据集";
        }
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //EXPORT_DATASET_REQUEST_DTO_HPP
