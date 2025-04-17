//
// Created by ender on 12/12/24.
//

#ifndef ANNOTATIONLOG_HPP
#define ANNOTATIONLOG_HPP
#include "service/ServiceBean.hpp"
#include "dto/data/task/AnnotationLogDto.hpp"

namespace ender_label::service::dataset::task {
    constexpr char table_name_annotation_log[] = "ender_label_annotation_log";

    class AnnotationLog final : public ServiceBean<table_name_annotation_log, data::task::AnnotationLogDto, Int64> {
    public:
    };
}

#endif //ANNOTATIONLOG_HPP
