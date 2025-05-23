#ifndef ANNOTATION_TASK_DTO_HPP
#define ANNOTATION_TASK_DTO_HPP
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::task {
    ENUM(AnnoTaskType, v_int32,
         VALUE(designated_image, 0, "designated_image"),
         VALUE(quantity, 1, "quantity"))

    ENUM(Function, v_int32,
         VALUE(segment,0,"segment"),
         VALUE(pose,1,"pose"),
         VALUE(detect,2,"detect"),
         VALUE(classify,3,"classify"))

    class AnnotationTaskDto : public oatpp::DTO {
        DTO_INIT(AnnotationTaskDto, DTO)
        DTO_FIELD(Int32, id);

        DTO_FIELD_INFO(id) {
            info->description = "TaskId任务ID";
        }

        DTO_FIELD(String, name);

        DTO_FIELD_INFO(name) {
            info->description = "任务名称";
        }

        DTO_FIELD(UnorderedSet<Int32>, user_ids);

        DTO_FIELD_INFO(user_ids) {
            info->description = "任务绑定的用户id列表，一个任务可以被分配给多个用户，如果这个字段为null则代表该任务是公共任务不限定用户";
        }

        DTO_FIELD(Int32, dataset_id);

        DTO_FIELD_INFO(dataset_id) {
            info->description = "数据集id";
        }

        DTO_FIELD(String, raw_json);

        DTO_FIELD_INFO(raw_json) {
            info->description = "储存任务信息的原始字符串，json字符串";
        }

        DTO_FIELD(Boolean, state);

        DTO_FIELD_INFO(state) {
            info->description = "任务是否完成，true代表未完成，false代表已经完成";
        }

        DTO_FIELD(Float32, progress);

        DTO_FIELD_INFO(progress) {
            info->description = "一个0-1之间的浮点数，代表任务进度，换算成百分比使用。";
        }

        DTO_FIELD(Enum<AnnoTaskType>::AsString, anno_task_type);

        DTO_FIELD_INFO(anno_task_type) {
            info->description = "标注任务类型，代表任务是以何种方式计量完成";
        }

        DTO_FIELD(Enum<Function>::AsString, anno_function);

        DTO_FIELD_INFO(anno_function) {
            info->description = "标注任务的功能";
        }
    };

    class AnnotationTaskConfigDto : public oatpp::DTO {
        DTO_INIT(AnnotationTaskConfigDto, DTO)

        DTO_FIELD(Boolean, match_read) = true;

        DTO_FIELD(Boolean, match_create) = true;

        DTO_FIELD(Boolean, match_delete) = true;

        DTO_FIELD(Boolean, match_update) = true;
    };


    class AnnotationTaskDataDto : public oatpp::DTO {
        DTO_INIT(AnnotationTaskDataDto, DTO)
        DTO_FIELD(Object<AnnotationTaskConfigDto>, config) = {};
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //ANNOTATION_TASK_DTO_HPP
