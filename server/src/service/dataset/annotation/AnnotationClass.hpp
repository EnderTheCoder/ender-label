//
// Created by ender on 11/29/24.
//

#ifndef ANNOTATIONCLASS_HPP
#define ANNOTATIONCLASS_HPP
#include "AnnotationClass.hpp"
#include "dto/data/annotation/AnnotationClassDto.hpp"

namespace ender_label::service::dataset::annotation {
    using namespace data::annotation;
    constexpr char table_name_anno_cls[] = "annotation_class";

    class AnnotationClass final : public ServiceBean<table_name_anno_cls, AnnotationClassDto> {
    };
}

#endif //ANNOTATIONCLASS_HPP
