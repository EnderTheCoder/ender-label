#ifndef DESIGNATED_IMAGE_TASK_DTO_HPP
#define DESIGNATED_IMAGE_TASK_DTO_HPP
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::task {
    class DesignatedImageTaskDto : public oatpp::DTO {
        DTO_INIT(DesignatedImageTaskDto, DTO)
        DTO_FIELD(Object<AnnotationTaskConfigDto>, config);
        DTO_FIELD(List<Object<AnnotationLogDto>>, logs);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //DESIGNATED_IMAGE_TASK_DTO_HPP
