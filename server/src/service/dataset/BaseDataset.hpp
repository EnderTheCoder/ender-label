//
// Created by ender on 11/26/24.
//

#ifndef BASEDATASET_HPP
#define BASEDATASET_HPP

#include "service/ServiceBean.hpp"
#include "dto/data/DatasetDto.hpp"
#include <boost/filesystem.hpp>

#include "annotation/SegmentationAnnotation.hpp"
#include "annotation/AnnotationClass.hpp"

#include <opencv2/opencv.hpp>

namespace ender_label::service::dataset {
    constexpr char table_name[] = "dataset";

    class BaseDataset : public ServiceBean<table_name, data::DatasetDto> {
    public:
        boost::filesystem::path root() {
            using namespace boost::filesystem;
            const OATPP_COMPONENT(oatpp::Object<data::ConfigDto>, config);
            return path(config->storage) / path("databases") / path(std::to_string(this->getDto()->id));
        }

        virtual void importYolo(const std::string &s_path,
                                const std::unordered_set<std::string> &supported_img_ext,
                                const std::unordered_set<std::string> &supported_anno_ext,
                                const std::string &type) {
            using namespace boost::filesystem;
            const path root(s_path);
            auto img_paths = std::set<path>{};
            auto anno_paths = std::set<path>{};
            std::function<void(path)> func_list_dir = [&](const auto& _path) {
                for (directory_iterator it(_path); it != directory_iterator(); ++it) {
                    if (is_regular_file(it->path())) {
                        if (supported_img_ext.contains(extension(it->path()))) {
                            img_paths.emplace(it->path());
                        }
                        if (supported_anno_ext.contains(extension(it->path()))) {
                            anno_paths.emplace(it->path());
                        }
                    } else if (is_directory(it->path())) {
                        func_list_dir(it->path());
                    }
                }
            };
            func_list_dir(root);
            auto func_import_anno = [&type, this](auto &img_p, auto &anno_p) {
                using namespace annotation;
                if (type == "segmentation") {
                    const auto img = cv::imread(img_p.c_str());

                    if (img.empty()) {
                        throw std::runtime_error("Failed to load img from " + img_p.string() + ": broken.");
                    }

                    auto source = String::loadFromFile(anno_p.c_str());
                    const auto anno_dto = data::AnnotationDto::createShared();
                    anno_dto->anno_cls_ids = this->getDto()->class_ids;
                    anno_dto->img_name = basename(img_p);
                    anno_dto->type = type;
                    anno_dto->owner_id = this->getDto()->owner_id;
                    anno_dto->width = img.cols;
                    anno_dto->height = img.rows;


                    auto annotation = SegmentationAnnotation::createShared<SegmentationAnnotation>(anno_dto);
                    annotation->fromYolo(source);
                    annotation->write();
                }
            };

            for (const auto &img_path: img_paths) {
                copy_file(img_path, this->root());
                for (const auto &ext: supported_anno_ext) {
                    if (const auto full_p = path(img_path.stem().string() + ext); anno_paths.contains(full_p)) {
                        func_import_anno(img_path, full_p);
                    }
                }
            }
        }

        virtual void importCoco(const std::string &s_path) {
        }

        virtual void importLabelme(const std::string &s_path) {
        }

        virtual void importVoc(const std::string &s_path) {
        }
    };
}

#endif //BASEDATASET_HPP
