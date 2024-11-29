//
// Created by ender on 11/26/24.
//

#ifndef SEGMENTATION_ANNOTATION_HPP
#define SEGMENTATION_ANNOTATION_HPP
#include "Annotation.hpp"

namespace ender_label::service::dataset::annotation {
    class SegmentationAnnotation : public Annotation {
        auto get_annotation() {
            const OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, mapper);
            return mapper->readFromString<Object<data::annotation::SegmentationDto> >(this->getDto()->raw_json);
        }

    public:
        std::string to_yolo() override {
            std::stringstream ss;
            for (const auto anno = this->get_annotation(); const auto &polygon: *anno->polygons) {
                ss << polygon->cls_id << " ";
                for (const auto &point: *polygon->normalized_points) {
                    ss << point->front() << " " << point->back();
                }
                ss << " \n";
            }
            return ss.str();
        }

        void from_yolo(const std::string &source) override {
            const OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, mapper);
            const auto dto = data::annotation::SegmentationDto::createShared();
            dto->polygons = {};
            std::vector<std::string> lines;
            boost::split(lines, source, boost::is_any_of("\n"));
            for (const auto &line: lines) {
                const auto polygon_dto = data::annotation::SegmentationDto::PolygonDto::createShared();
                std::vector<std::string> nums;
                boost::split(nums, line, boost::is_any_of(" "));
                const auto cls_id = std::stoi(nums.front());
                polygon_dto->normalized_points = {};
                for (auto it_x = nums.begin() + 1; it_x < nums.end(); it_x += 2) {
                    auto point = List<Float64>::createShared();
                    point->resize(2);
                    point[0] = std::stod(*it_x); // x
                    point[1] = std::stod(*(it_x + 1)); // y
                    polygon_dto->normalized_points->emplace_back(point);
                }
                polygon_dto->cls_id = cls_id;
                dto->polygons->emplace_back(polygon_dto);
            }
            this->getDto()->raw_json = mapper->writeToString(dto);
        }
    };
}


#endif //SEGMENTATION_ANNOTATION_HPP
