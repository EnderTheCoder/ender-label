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

#include "service/user/Permission.hpp"
#include "service/user/User.hpp"
#include "Dataset.hpp"
#include "Image.hpp"
#include <opencv2/opencv.hpp>

namespace ender_label::service::dataset {
    namespace fs = std::filesystem;
    constexpr char table_name[] = "ender_label_img_dataset";

    class ImageDataset final : public ServiceBean<table_name, data::ImageDatasetDto>, public Dataset {
    public:
        std::filesystem::path root() {
            using namespace std::filesystem;
            const OATPP_COMPONENT(oatpp::Object<data::ConfigDto>, config);
            return path(config->storage) / "datasets" / path(std::to_string(this->getDto()->id));
        }

        virtual void importYolo(const std::string &s_path,
                                const std::unordered_set<std::string> &supported_img_ext,
                                const std::unordered_set<std::string> &supported_anno_ext,
                                const std::string &task_type) {
            using namespace std::filesystem;
            const path root(s_path);
            auto img_paths = std::set<path>{};
            auto anno_paths = std::set<path>{};
            std::function<void(path)> func_list_dir = [&](const auto &_path) {
                for (directory_iterator it(_path); it != directory_iterator(); ++it) {
                    if (is_regular_file(it->path())) {
                        if (supported_img_ext.contains(it->path().extension().string())) {
                            img_paths.emplace(it->path());
                        }
                        if (supported_anno_ext.contains(it->path().extension().string())) {
                            anno_paths.emplace(it->path());
                        }
                    } else if (is_directory(it->path())) {
                        func_list_dir(it->path());
                    }
                }
            };
            func_list_dir(root);
            auto func_import_anno = [&task_type, this](auto &img_p, auto &anno_p) {
                using namespace annotation;
                if (task_type == "segment") {
                    const auto img = cv::imread(img_p.c_str());

                    if (img.empty()) {
                        throw std::runtime_error("Failed to load img from " + img_p.string() + ": broken.");
                    }

                    auto source = String::loadFromFile(anno_p.c_str());
                    const auto anno_dto = data::AnnotationDto::createShared();
                    anno_dto->anno_cls_ids = this->getDto()->class_ids;
                    anno_dto->img_name = img_p.filename().string();
                    anno_dto->task_type = task_type;
                    anno_dto->owner_id = this->getDto()->owner_id;
                    // anno_dto->width = img.cols;
                    // anno_dto->height = img.rows;


                    auto annotation = SegmentationAnnotation::createShared<SegmentationAnnotation>(anno_dto);
                    annotation->fromYolo(source);
                    annotation->write();
                }
            };

            for (const auto &img_path: img_paths) {
                try {
                    if (exists(this->root() / img_path.filename())) {
                        OATPP_LOGI("DATASET", "Skipping import existing img file: %s", img_path.filename().c_str())
                        continue;
                    }
                    copy_file(img_path, this->root() / img_path.filename());
                    for (const auto &ext: supported_anno_ext) {
                        if (const auto full_p = path(img_path.stem().string() + ext); anno_paths.contains(full_p)) {
                            func_import_anno(img_path, full_p);
                            break;
                        }
                    }
                } catch (filesystem_error &e) {
                    OATPP_LOGE("DATASET", "Exception while importing image file %s to dataset %d: %s",
                               img_path.c_str(), *this->getId(), e.what())
                }
            }
        }

        virtual void importCoco(const std::string &s_path) {
        }

        virtual void importLabelme(const std::string &s_path) {
        }

        virtual void importVoc(const std::string &s_path) {
        }

        virtual void exportYolo() {
        }

        virtual void exportCoco() {
        }

        virtual void exportLabelme() {
        }

        virtual void exportVoc() {
        }

        void exportVanilla() override {
        }

        void importVanilla() override {
        }

        std::string getUpdatePermKey() {
            return "DATASET_UPDATE_[" + std::to_string(this->getId()) + "]";
        }

        std::string getDeletePermKey() {
            return "DATASET_DELETE_[" + std::to_string(this->getId()) + "]";
        }

        std::string getReadPermKey() {
            return "DATASET_READ_[" + std::to_string(this->getId()) + "]";
        }

        std::vector<std::string> getPermKeys() {
            auto keys = std::vector{getUpdatePermKey(), getDeletePermKey(), getReadPermKey()};
            return keys;
        }

        auto getStorageDir() {
            OATPP_COMPONENT(oatpp::Object<data::ConfigDto>, config);
            const auto base = fs::path(config->storage);
            return base / "datasets" / std::to_string(*this->getId());
        }

        void initStorage() {
            if (const auto path = this->getStorageDir(); not exists(path)) {
                create_directory(path);
            } else {
                OATPP_LOGW("DATASET", "Skipping init existing dataset storage path: %s", path.c_str())
            }
        }
    };
}

#endif //BASEDATASET_HPP
