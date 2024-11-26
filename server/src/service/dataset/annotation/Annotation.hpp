//
// Created by ender on 11/26/24.
//

#ifndef ANNOTATION_HPP
#define ANNOTATION_HPP

#include "service/ServiceBean.hpp"
#include "dto/data/AnnotationDto.hpp"
#include "dto/data/annotation/SegmentationDto.hpp"
#include "dto/data/annotation/PoseDto.hpp"
#include "dto/data/annotation/ObjectDetectionDto.hpp"
namespace ender_label::service::dataset::annotation {
    constexpr char table_name[] = "annotation";

    class Annotation : public ServiceBean<table_name, data::AnnotationDto> {
    public:

    };
}


#endif //ANNOTATION_HPP
