//
// Created by ender on 11/26/24.
//

#ifndef ANNOTATION_HPP
#define ANNOTATION_HPP

#include <utility>

#include "service/ServiceBean.hpp"
#include "dto/data/annotation/AnnotationDto.hpp"
#include "dto/data/annotation/SegmentationDto.hpp"
#include "dto/data/annotation/PoseDto.hpp"
#include "dto/data/annotation/ObjectDetectionDto.hpp"
#include "AnnotationClass.hpp"

namespace ender_label::service::dataset::annotation {
    constexpr char table_name[] = "ender_label_img_dataset_annotation";

    class Annotation : public ServiceBean<table_name, AnnotationDto, Int64> {
    public:
        explicit Annotation(const auto &dto): ServiceBean(dto) {
        }

        virtual std::string toYolo() = 0;

        virtual std::string toCoco() = 0;

        virtual std::string toVoc() = 0;

        virtual std::string toLabelme() = 0;

        virtual void fromYolo(const std::string &source) = 0;

        virtual void fromCoco(const std::string &source) = 0;

        virtual void fromLabelme(const std::string &source) = 0;

        virtual void fromVoc(const std::string &source) = 0;
    };

    class TransException : public std::runtime_error {
    public:
        std::string from, to;

        explicit TransException(const std::string &from, const std::string &to)
            : std::runtime_error("Exception in transforming annotation: from " + from + " to " + to + ".\n"),
              from(from),
              to(to) {
        }

        explicit TransException(std::string from, std::string to, const std::string &cause)
            : std::runtime_error(cause),
              from(std::move(from)),
              to(std::move(to)) {
        }
    };

    class LabelNotFoundException final : public TransException {
    public:
        std::string label;

        explicit LabelNotFoundException(const std::string &from, const std::string &to, std::string label)
            : TransException(from, to, "Label not found: " + label), label(std::move(label)) {
        }
    };

    class NotImplException final : public TransException {
    public:
        explicit NotImplException(const std::string &from, const std::string &to)
            : TransException(from, to, "Trans from " + from + " to " + to + " is not implemented.") {
        }
    };
}


#endif //ANNOTATION_HPP
