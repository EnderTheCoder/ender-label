#ifndef DESIGNATED_IMAGE_TASK_DTO_HPP
#define DESIGNATED_IMAGE_TASK_DTO_HPP
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>

#include "AnnotationTaskDto.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::task {
    class DesignatedImageTaskDto final : public AnnotationTaskDataDto {
        DTO_INIT(DesignatedImageTaskDto, AnnotationTaskDataDto)

        DTO_FIELD(UnorderedSet<Int64>, img_ids) = {};

        DTO_FIELD(UnorderedSet<Int64>, target_img_ids) = {};
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //DESIGNATED_IMAGE_TASK_DTO_HPP
