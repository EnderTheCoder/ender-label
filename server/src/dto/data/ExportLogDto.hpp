#ifndef EXPORT_LOG_DTO_HPP
#define EXPORT_LOG_DTO_HPP
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

namespace ender_label::dto::data {
    ENUM(ExportLogType, v_int32,
         VALUE(WAITING,0), VALUE(WORKING,1), VALUE(EXCEPTION, 2), VALUE(COMPLETED,3), VALUE(TIMEOUT,4))

    class ExportLogDto : public oatpp::DTO {
        DTO_INIT(ExportLogDto, DTO)
        DTO_FIELD(Int32, id);

        DTO_FIELD_INFO(id) {
            info->description = "导出记录ID export_id";
        }

        DTO_FIELD(Int32, dataset_id);

        DTO_FIELD_INFO(dataset_id) {
            info->description = "数据集id";
        }

        DTO_FIELD(String, archive_path);

        DTO_FIELD_INFO(archive_path) {
            info->description = "导出的文件路径";
        }

        DTO_FIELD(Int32, user_id);

        DTO_FIELD_INFO(user_id) {
            info->description = "用户id";
        }

        DTO_FIELD(Int64, begin_time);

        DTO_FIELD_INFO(begin_time) {
            info->description = "开始导出的时间";
        }

        DTO_FIELD(Int64, end_time);

        DTO_FIELD_INFO(end_time) {
            info->description = "结束导出的时间";
        }

        DTO_FIELD(Enum<ExportLogType>, state);

        DTO_FIELD_INFO(state) {
            info->description = "导出状态";
        }

        DTO_FIELD(String, info);

        DTO_FIELD_INFO(info) {
            info->description = "导出信息，如果出现异常会在这里显示原因";
        }
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //EXPORT_LOG_DTO_HPP
