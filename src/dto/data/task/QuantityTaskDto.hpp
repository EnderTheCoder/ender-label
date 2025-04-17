#ifndef QUANTITY_TASK_DTO_HPP
#define QUANTITY_TASK_DTO_HPP
#include <oatpp/codegen/dto/base_define.hpp>

#include "AnnotationTaskDto.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::task {
    class QuantityTaskDto final : public AnnotationTaskDataDto {
        DTO_INIT(QuantityTaskDto, AnnotationTaskDataDto)

        DTO_FIELD(Int32, amount);

        DTO_FIELD(Int32, target_amount);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //QUANTITY_TASK_DTO_HPP
