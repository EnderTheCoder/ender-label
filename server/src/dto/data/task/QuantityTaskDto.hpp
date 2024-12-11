#ifndef QUANTITY_TASK_DTO_HPP
#define QUANTITY_TASK_DTO_HPP
#include <oatpp/codegen/dto/base_define.hpp>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::task {
    class QuantityTaskDto : public oatpp::DTO {
        DTO_INIT(QuantityTaskDto, DTO)

        DTO_FIELD(Object<AnnotationTaskConfigDto>, config);

        DTO_FIELD(List<Object<AnnotationLogDto>>, logs);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //QUANTITY_TASK_DTO_HPP