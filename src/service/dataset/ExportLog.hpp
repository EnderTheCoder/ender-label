//
// Created by ender on 12/14/24.
//

#ifndef EXPORTLOG_HPP
#define EXPORTLOG_HPP

#include "dto/data/ExportLogDto.hpp"
#include "service/ServiceBean.hpp"

namespace ender_label::service::dataset {
    constexpr char table_name_export_log[] = "ender_label_export_log";

    class ExportLog final : public ServiceBean<table_name_export_log, data::ExportLogDto> {
    public:
    };
}


#endif //EXPORTLOG_HPP
