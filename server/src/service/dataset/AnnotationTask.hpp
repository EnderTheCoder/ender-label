//
// Created by ender on 12/11/24.
//

#ifndef ANNOTATIONTASK_HPP
#define ANNOTATIONTASK_HPP


#include "service/ServiceBean.hpp"
#include "dto/data/annotation/AnnotationTaskDto.hpp"

namespace ender_label::service::dataset {
    constexpr char table_name_annotation_task[] = "ender_label_annotation_task";
    template<typename TASK_DTO_TYPE>
    class AnnotationTask : public ServiceBean<table_name_annotation_task, annotation::AnnotationTaskDto>{

    };
}


#endif //ANNOTATIONTASK_HPP
