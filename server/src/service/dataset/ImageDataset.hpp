//
// Created by ender on 11/26/24.
//

#ifndef BASEDATASET_HPP
#define BASEDATASET_HPP

#include "service/ServiceBean.hpp"
#include "dto/data/DatasetDto.hpp"

#include "annotation/SegmentationAnnotation.hpp"
#include "annotation/AnnotationClass.hpp"

#include "service/user/Permission.hpp"
#include "service/user/User.hpp"
#include "Dataset.hpp"
#include "Image.hpp"
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>

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

        void importYolo(const std::string &s_path,
                        const std::unordered_set<std::string> &supported_img_ext,
                        const std::unordered_set<std::string> &supported_anno_ext,
                        const TaskType &task_type) {
            using namespace std::filesystem;
            const path root(s_path);
            auto img_paths = std::set<path>{};
            auto stem_to_anno_paths = std::unordered_map<path, path>{};
            std::function<void(path)> func_list_dir = [&](const auto &_path) {
                for (directory_iterator it(_path); it != directory_iterator(); ++it) {
                    if (is_regular_file(it->path())) {
                        if (supported_img_ext.contains(it->path().extension().string())) {
                            img_paths.emplace(it->path());
                        }
                        if (supported_anno_ext.contains(it->path().extension().string())) {
                            stem_to_anno_paths.emplace(it->path().stem(), it->path());
                        }
                    } else if (is_directory(it->path())) {
                        func_list_dir(it->path());
                    }
                }
            };
            func_list_dir(root);
            auto func_import_anno = [&task_type, this](auto &img_p, auto &anno_p, auto &img_id) {
                using namespace annotation;
                if (task_type == TaskType::segment) {
                    if (const auto img = cv::imread(img_p.c_str()); img.empty()) {
                        throw std::runtime_error("Failed to load img from " + img_p.string() + ": broken.");
                    }

                    auto source = String::loadFromFile(anno_p.c_str());
                    const auto anno_dto = AnnotationDto::createShared();
                    anno_dto->anno_cls_ids = this->getDto()->class_ids;
                    anno_dto->img_id = img_id;
                    anno_dto->task_type = task_type;
                    anno_dto->owner_id = this->getDto()->owner_id;
                    // anno_dto->width = img.cols;
                    // anno_dto->height = img.rows;


                    auto annotation = SegmentationAnnotation::createShared<SegmentationAnnotation>(anno_dto);
                    annotation->fromYolo(source);
                    annotation->write();
                }
            };
            auto n_dto = data::ImageDatasetDto::createShared();
            n_dto->img_ids = this->getDto()->img_ids;
            for (const auto &img_path: img_paths) {
                try {
                    auto dst = this->root() / img_path.filename();
                    if (exists(dst)) {
                        OATPP_LOGI("DATASET", "Skipping import existing img file: %s", img_path.filename().c_str())
                        continue;
                    }
                    copy_file(img_path, dst);
                    auto img = Image::createFromFile(dst);
                    if (const auto stem = img_path.stem(); stem_to_anno_paths.contains(stem)) {
                        auto img_id = img->getId();
                        func_import_anno(img_path, stem_to_anno_paths.at(stem), img_id);
                        break;
                    }
                    if (n_dto->img_ids == nullptr) {
                        n_dto->img_ids = {};
                    }
                    n_dto->img_ids->emplace(img->getId());
                } catch (filesystem_error &e) {
                    OATPP_LOGE("DATASET", "Exception while importing image file %s to dataset %d: %s",
                               img_path.c_str(), *this->getId(), e.what())
                }
            }
            this->overwrite(n_dto);
        }

        auto getAllAnnoWithType(const Enum<TaskType>::AsString &type) {
            const auto res = this->db->executeQuery("SELECT ender_label_img_dataset_annotation.* "
                                                    "FROM ender_label_img_dataset_annotation "
                                                    "JOIN ender_label_img ON ender_label_img.id = ender_label_img_dataset_annotation.img_id "
                                                    "JOIN ender_label_img_dataset ON ender_label_img.id = ANY (ender_label_img_dataset.img_ids) "
                                                    "WHERE ender_label_img_dataset.id = :dataset_id AND ender_label_img_dataset_annotation.task_type = :task_type",
                                                    {
                                                        {"dataset_id", this->getId()},
                                                        {"task_type", type}
                                                    });
            return annotation::Annotation::toDtoList(annotation::Annotation::getList(res));
        }

        auto getAllImage() {
            const auto res = this->db->executeQuery(
                "SELECT ender_label_img.* FROM ender_label_img "
                "JOIN ender_label_img_dataset ON ender_label_img.id = ANY(ender_label_img_dataset.img_ids) "
                "WHERE ender_label_img_dataset.id = :dataset_id", {
                    {"dataset_id", this->getId()},
                });
            return Image::toDtoList(Image::getList(res));
        }

        void importCoco(const std::string &s_path) {
        }

        void importLabelme(const std::string &s_path) {
        }

        void importVoc(const std::string &s_path) {
        }

        void exportYolo(const std::filesystem::path &export_path, const TaskType &task_type,
                        const auto &annotated_only) {
            if (not exists(export_path)) {
                create_directory(export_path);
            }
            const auto label_root = export_path / "labels";
            const auto image_root = export_path / "images";

            if (not exists(label_root)) {
                create_directory(label_root);
            }
            if (not exists(image_root)) {
                create_directory(image_root);
            }

            boost::asio::thread_pool anno_pool(16);
            auto img_dtos_f = std::async(std::launch::async, [this] {
                return getAllImage();
            });
            const auto anno_dtos = getAllAnnoWithType(task_type);
            std::unordered_set<int64_t> img_ids = {};
            std::mutex img_ids_lck;
            for (const auto &anno_dto: *anno_dtos) {
                boost::asio::post(anno_pool, [task_type, &img_ids, anno_dto, &label_root, &img_ids_lck]() {
                    String anno_str{};
                    switch (task_type) {
                        case TaskType::segment: {
                            const auto anno = annotation::SegmentationAnnotation::createShared<>(anno_dto);
                            anno_str = anno->toYolo();
                            break;
                        }
                        default:
                            throw std::runtime_error("Task type is not implemented.");
                    } {
                        std::lock_guard guard(img_ids_lck);
                        img_ids.emplace(anno_dto->img_id);
                    }
                    anno_str.saveToFile((label_root / (std::to_string(anno_dto->img_id) + ".txt")).c_str());
                });
            }
            anno_pool.join();
            boost::asio::thread_pool img_pool(16);
            const auto img_dtos = img_dtos_f.get();
            for (const auto &img_dto: *img_dtos | std::views::filter([&annotated_only, &img_ids](auto &x) {
                if (annotated_only) {
                    return img_ids.contains(*x->id);
                }
                return true;
            })) {
                boost::asio::post(img_pool, [&img_dto, &image_root]() {
                    copy_file(std::filesystem::path(*img_dto->relative_path), image_root);
                });
            }
            img_pool.join();
        }

        void exportCoco() {
        }

        void exportLabelme() {
        }

        void exportVoc() {
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

        // auto getStorageDir() {
        //     OATPP_COMPONENT(oatpp::Object<data::ConfigDto>, config);
        //     const auto base = fs::path(config->storage);
        //     return base / "datasets" / std::to_string(*this->getId());
        // }

        void initStorage() {
            const auto path = this->root();
            if (not exists(path)) {
                create_directory(path);
            } else {
                OATPP_LOGW("DATASET", "Skipping init existing dataset storage path: %s", path.c_str())
            }
            if (not exists(path / "exports")) {
                create_directory(path / "exports");
            }
        }

        auto getClasses() {
            const auto vec = Vector<Object<annotation::AnnotationClassDto> >::createShared();
            for (const auto &anno_cls_id: *this->getDto()->class_ids) {
                const auto anno_cls = annotation::AnnotationClass::getById(anno_cls_id);
                if (anno_cls == nullptr) {
                    throw std::runtime_error(
                        "AnnotationClass[id:" + std::to_string(anno_cls_id) + "] not found, broken data.");
                }
                vec->push_back(anno_cls->getDto());
            }
            return vec;
        }
    };
}

#endif //BASEDATASET_HPP
