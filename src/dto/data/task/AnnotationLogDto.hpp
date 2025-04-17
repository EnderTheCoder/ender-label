#ifndef ANNOTATION_LOG_DTO_HPP
#define ANNOTATION_LOG_DTO_HPP
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data::task {
    ENUM(AnnoLogType, v_int32, VALUE(READ, 0), VALUE(CREATE, 1), VALUE(DELETE, 2), VALUE(UPDATE, 3))

    class AnnotationLogDto : public oatpp::DTO {
        DTO_INIT(AnnotationLogDto, DTO)

        DTO_FIELD(Int64, id);

        DTO_FIELD(Int64, time);

        DTO_FIELD(Int32, task_id);

        DTO_FIELD(Int64, anno_id);

        DTO_FIELD(Int64, img_id);

        DTO_FIELD(Int32, user_id);

        DTO_FIELD(Enum<AnnoLogType>::AsString, log_type);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //ANNOTATION_LOG_DTO_HPP