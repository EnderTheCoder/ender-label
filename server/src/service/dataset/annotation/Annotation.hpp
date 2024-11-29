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
        virtual std::string to_yolo() = 0;

        virtual std::string to_coco() = 0;

        virtual std::string to_voc() = 0;

        virtual std::string to_labelme() = 0;

        virtual void from_yolo(const std::string &source) = 0;

        virtual void from_coco(const std::string &source) = 0;

        virtual void from_labelme(const std::string &source) = 0;

        virtual void from_voc(const std::string &source) = 0;
    };
}


#endif //ANNOTATION_HPP
