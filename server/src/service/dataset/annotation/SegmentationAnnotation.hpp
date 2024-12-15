//
// Created by ender on 11/26/24.
//

#ifndef SEGMENTATION_ANNOTATION_HPP
#define SEGMENTATION_ANNOTATION_HPP
#include "Annotation.hpp"
#include "dto/data/annotation/foreign/LabelmeSegmentationDto.hpp"

namespace ender_label::service::dataset::annotation {
    using namespace data::annotation;

    class SegmentationAnnotation final : public Annotation {
        auto get_annotation() {
            const OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, mapper);
            return mapper->readFromString<Object<SegmentationDto> >(this->getDto()->raw_json);
        }

        auto clsKeyToIdx(const auto &key) {
            auto idx = 0;
            for (; idx < this->getDto()->anno_cls_ids->size(); idx++) {
                if (AnnotationClass::getById(this->getDto()->anno_cls_ids[idx])->getDto()->key == key) {
                    break;
                }
                if (idx == this->getDto()->anno_cls_ids->size() - 1) {
                    throw LabelNotFoundException("", "", key);
                }
            }
            return idx;
        }

        auto clsIdToSerial(const auto &id) {
            auto idx = 0;
            for (auto it = this->getDto()->anno_cls_ids->begin(); it != this->getDto()->anno_cls_ids->end();
                 ++it, ++idx) {
                if (*it == id) {
                    return idx;
                }
            }
            throw LabelNotFoundException("", "", std::to_string(id));
        }

        auto serialToClsId(const auto &serial) {
            if (serial >= this->getDto()->anno_cls_ids->size() or serial < 0) {
                throw LabelNotFoundException("", "", std::to_string(serial));
            }
            return this->getDto()->anno_cls_ids[serial];
        }

    public:
        explicit SegmentationAnnotation(const auto &dto): Annotation(dto) {
        }

        template<typename T=SegmentationAnnotation>
        static auto createShared(const auto &dto) {
            return std::make_shared<SegmentationAnnotation>(dto);
        }

        std::string toYolo() override {
            std::stringstream ss;
            for (const auto anno = this->get_annotation(); const auto &polygon: *anno->polygons) {
                ss << clsIdToSerial(polygon->cls_id) << " ";
                for (const auto &point: *polygon->normalized_points) {
                    ss << point->front() << " " << point->back() << " ";
                }
                ss << "\n";
            }
            return ss.str();
        }

        void fromYolo(const std::string &source) override {
            const OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, mapper);
            const auto dto = SegmentationDto::createShared();
            dto->polygons = {};
            std::vector<std::string> lines;
            split(lines, source, boost::is_any_of("\n"));
            for (const auto &line: lines) {
                if (line == "") continue;
                const auto polygon_dto = SegmentationDto::PolygonDto::createShared();
                std::vector<std::string> nums;
                split(nums, line, boost::is_any_of(" "));
                auto cls_id = 0;
                try {
                    cls_id = std::stoi(nums.front());
                } catch (std::invalid_argument &e) {
                    throw std::invalid_argument(
                        "Invalid literal cls id " + nums.front() + " while parsing annotation.");
                }
                polygon_dto->normalized_points = {};
                if ((nums.size() - 1) % 2 != 0) {
                    throw std::invalid_argument("Invalid point xy pairs: size is not dividable by 2.");
                }
                if ((nums.size() - 1) / 2 < 3) {
                    throw std::invalid_argument("An polygon need at least 3 points");
                }
                for (auto it_x = nums.begin() + 1; it_x < nums.end(); it_x += 2) {
                    auto point = List<Float64>::createShared();
                    point->resize(2);
                    point[0] = std::stod(*it_x); // x
                    point[1] = std::stod(*(it_x + 1)); // y
                    polygon_dto->normalized_points->emplace_back(point);
                }
                if (cls_id >= this->getDto()->anno_cls_ids->size()) {
                    throw LabelNotFoundException("yolo", "vanilla", std::to_string(cls_id));
                }
                polygon_dto->cls_id = this->serialToClsId(cls_id);
                dto->polygons->emplace_back(polygon_dto);
            }
            this->getDto()->raw_json = mapper->writeToString(dto);
        }

        void fromLabelme(const std::string &source) override {
            const OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, mapper);
            const auto dto = SegmentationDto::createShared();
            const auto foreign_dto = mapper->readFromString<Object<
                foreign::LabelmeSegmentationDto> >(source);
            for (const auto &shape: *foreign_dto->shapes) {
                const auto polygon_dto = SegmentationDto::PolygonDto::createShared();
                try {
                    polygon_dto->cls_id = clsKeyToIdx(shape->label);
                } catch (LabelNotFoundException &e) {
                    e.from = "labelme";
                    e.to = "vanilla";
                    throw;
                }
                for (const auto &point: *shape->points) {
                    const auto x = point[0] / static_cast<double>(foreign_dto->imageWidth);
                    const auto y = point[1] / static_cast<double>(foreign_dto->imageHeight);
                    const auto n_point = List<Float64>::createShared();
                    n_point->resize(2);
                    n_point[0] = x;
                    n_point[1] = y;
                    polygon_dto->normalized_points->emplace_back(n_point);
                }
                dto->polygons->emplace_back(polygon_dto);
            }
            // this->getDto()->width = foreign_dto->imageWidth;
            // this->getDto()->height = foreign_dto->imageHeight;
        }

        std::string toLabelme() override {
            const OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, mapper);
            const auto dto = foreign::LabelmeSegmentationDto::createShared();
            //todo: vanilla -> labelme conv
            throw NotImplException("vanilla", "labelme");
            return mapper->writeToString(dto);
        }

        void fromCoco(const std::string &source) override {
            throw NotImplException("coco", "vanilla");
        }

        void fromVoc(const std::string &source) override {
            throw NotImplException("voc", "vanilla");
        }

        std::string toCoco() override {
            throw NotImplException("vanilla", "coco");
        }

        std::string toVoc() override {
            throw NotImplException("vanilla", "voc");
        }
    };
}


#endif //SEGMENTATION_ANNOTATION_HPP
