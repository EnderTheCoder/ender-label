#ifndef IMPORT_DATASET_REQUEST_DTO_HPP
#define IMPORT_DATASET_REQUEST_DTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::request {
    class ImportDatasetRequestDto final : public oatpp::DTO {
        DTO_INIT(ImportDatasetRequestDto, DTO)

        DTO_FIELD(String, task_type) = "segment";

        DTO_FIELD_INFO(task_type) {
            info->description = "任务类型，支持：segment, classify, detect, pose";
            info->required = true;
        }

        DTO_FIELD(String, import_dir);

        DTO_FIELD_INFO(import_dir) {
            info->description = "导入扫描路径，必须为一个服务器上的绝对路径";
            info->required = true;
        }

        DTO_FIELD(String, anno_type) = "yolo";

        DTO_FIELD_INFO(anno_type) {
            info->description = "导入类型，支持：voc, coco, labelme, yolo";
            info->required = true;
        }

        DTO_FIELD(String, dataset_type) = "image";

        DTO_FIELD_INFO(dataset_type) {
            info->description = "数据集类型，当前仅支持image";
            info->required = true;
        }

        DTO_FIELD(List<String>, img_exts);

        DTO_FIELD_INFO(img_exts) {
            info->description = "图片拓展名，留空则使用默认";
        }

        DTO_FIELD(List<String>, anno_exts);

        DTO_FIELD_INFO(anno_exts) {
            info->description = "标注拓展名，留空则使用默认";
        }
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //IMPORT_DATASET_REQUEST_DTO_HPP
