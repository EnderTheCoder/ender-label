//
// Created by ender on 11/26/24.
//

#ifndef ANNOTATIONDTO_HPP
#define ANNOTATIONDTO_HPP


#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data {
    class AnnotationDto : public oatpp::DTO {
        DTO_INIT(AnnotationDto, DTO)
        DTO_FIELD(Int32, id);

        DTO_FIELD(Int32, owner_id);

        DTO_FIELD_INFO(owner_id) {
            info->description = "所有者用户id";
        }

        DTO_FIELD(String, img_name);

        DTO_FIELD_INFO(img_name) {
            info->description = "图片文件名";
        }

        DTO_FIELD(String, task_type);

        DTO_FIELD_INFO(task_type) {
            info->description = "任务类型";
        }

        DTO_FIELD(String, raw_json);

        DTO_FIELD_INFO(raw_json) {
            info->description = "标注原始字符串";
        }

        DTO_FIELD(List<Int32>, anno_cls_ids);

        DTO_FIELD_INFO(anno_cls_ids) {
            info->description = "标注使用的id列表";
        }
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //ANNOTATIONDTO_HPP
