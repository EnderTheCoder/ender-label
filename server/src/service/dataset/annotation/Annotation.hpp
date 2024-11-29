//
// Created by ender on 11/26/24.
//

#ifndef ANNOTATION_HPP
#define ANNOTATION_HPP

#include <sstream>

#include "service/ServiceBean.hpp"
#include "dto/data/AnnotationDto.hpp"
#include "dto/data/annotation/SegmentationDto.hpp"
#include "dto/data/annotation/PoseDto.hpp"
#include "dto/data/annotation/ObjectDetectionDto.hpp"
#include "AnnotationClass.hpp"

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

    class TransException : public std::runtime_error {
    public:
        std::string from, to;

        explicit TransException(const std::string &from, const std::string &to)
            : std::runtime_error("Exception in transforming annotation: from " + from + " to " + to + ".\n"),
              from(from),
              to(to) {
        }
    };

    class LabelNotFoundException final : public TransException {
    public:
        std::string label;

        explicit LabelNotFoundException(const std::string &from, const std::string &to, const std::string &label)
            : TransException(from, to), label(label) {
        }

        [[nodiscard]] const char *what() const noexcept override {
            return ("Label not found: " + this->label).c_str();
        };
    };
}


#endif //ANNOTATION_HPP
