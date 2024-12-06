//
// Created by ender on 11/26/24.
//

#ifndef DATASETCONTROLLER_HPP
#define DATASETCONTROLLER_HPP
#include "dto/request/ImportDatasetRequestDto.hpp"

#include <dto/response/SimpleDataResponseDto.hpp>

#include "service/dataset/ImageDataset.hpp"
#include "service/dataset/annotation/SegmentationAnnotation.hpp"
#include "service/dataset/annotation/AnnotationMerger.hpp"
#include "util/AuthUtil.hpp"
#include "util/ControllerUtil.hpp"
#include OATPP_CODEGEN_BEGIN(ApiController)

namespace ender_label::controller {
    using namespace service;

    class DatasetController final : public oatpp::web::server::api::ApiController {
    private:
        explicit DatasetController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper) {
            this->setErrorHandler(std::make_shared<component::ErrorHandler>(objectMapper));
        }

    public:
        static std::shared_ptr<DatasetController>
        createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
            return std::shared_ptr<DatasetController>(new DatasetController(objectMapper));
        }

    public:
        ENDPOINT("POST", "/dataset/create", createDataset, BODY_DTO(Object<data::ImageDatasetDto>, dto), AUTH_HEADER) {
            AUTH
            REQUEST_PARAM_CHECK(dto->class_ids)
            REQUEST_PARAM_CHECK(dto->desc)
            REQUEST_PARAM_CHECK(dto->name)
            OATPP_ASSERT_HTTP(USER->hasPerm("DATASET_ADD"), Status::CODE_403, "Permission denied.")
            const auto resp = SimpleDataResponseDto<Object<data::ImageDatasetDto> >::createShared();
            dto->owner_id = USER->getId();
            const auto dataset = dataset::ImageDataset::createShared<dataset::ImageDataset>(dto);
            dataset->write();
            dataset->initPerm();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(createDataset) {
            info->name = "创建数据集";
            info->description =
                    "创建数据集需要具备权限DATASET_ADD。"
                    "创建一个空的数据集，所有权归当前登录用户所有。"
                    "接收参数验证class_ids，desc，name字段非空。";
            info->addConsumes<Object<data::ImageDatasetDto> >("application/json");
            info->addResponse<Object<SimpleDataResponseDto<Object<data::ImageDatasetDto> > > >(
                Status::CODE_200, "application/json");
        }

        ENDPOINT("POST", "/dataset/{dataset_id}/import", importDataset, AUTH_HEADER,
                 PATH(Int32, dataset_id),
                 BODY_DTO(Object<request::ImportDatasetRequestDto>, dto)) {
            AUTH
            REQUEST_PARAM_CHECK(dto->import_dir)
            REQUEST_PARAM_CHECK(dto->task_type)
            REQUEST_PARAM_CHECK(dto->dataset_type)
            const auto resp = BaseResponseDto::createShared();
            const auto dataset = dataset::ImageDataset::getById<dataset::ImageDataset>(dataset_id);
            OATPP_ASSERT_HTTP(dataset != nullptr, Status::CODE_404, "Dataset does not exist.")
            OATPP_ASSERT_HTTP(USER->hasPerm("DATASET_UPDATE_["+std::to_string(dataset_id)+"]"), Status::CODE_200,
                              "Permission denied.")
            OATPP_ASSERT_HTTP(dto->task_type == "segment", Status::CODE_500, "Unimplemented task type")
            OATPP_ASSERT_HTTP(dto->dataset_type == "image", Status::CODE_500, "Unimplemented dataset type")
            OATPP_ASSERT_HTTP(dto->anno_type == "yolo", Status::CODE_500, "Unimplemented annotation type")
            auto img_exts = std::unordered_set<std::string>{};
            auto anno_exts = std::unordered_set<std::string>{};
            if (dto->img_exts == nullptr or dto->img_exts->empty()) {
                img_exts = {".jpg", ".png", ".JPG", ".PNG"};
            } else
                std::for_each(dto->img_exts->begin(), dto->img_exts->end(), [&img_exts](auto &x) {
                    OATPP_ASSERT_HTTP(x->size() > 1 and x->front() == '.', Status::CODE_400, "Wrong ext format.")
                    img_exts.emplace(x);
                }); {
            }
            if (dto->anno_exts == nullptr or dto->anno_exts->empty()) {
                if (dto->anno_type == "voc" or dto->anno_type == "labelme" or dto->anno_type == "coco") {
                    anno_exts = {".json"};
                } else if (dto->anno_type == "yolo") {
                    anno_exts = {".txt"};
                }
            } else {
                std::for_each(dto->anno_exts->begin(), dto->anno_exts->end(), [&anno_exts](auto &x) {
                    OATPP_ASSERT_HTTP(x->size() > 1 and x->front() == '.', Status::CODE_400, "Wrong ext format.")
                    anno_exts.emplace(x);
                });
            }
            dataset->importYolo(dto->import_dir, img_exts, anno_exts, "segmentation");
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(importDataset) {
            info->description =
                    "向一个已经存在的数据集导入数据。"
                    "需要确保目录具有访问权限。"
                    "需要确保目录中包含的图片和标注文件不含有重复文件名。"
                    "当前仅支持从服务器本地路径进行导入。";
            info->addConsumes<Object<request::ImportDatasetRequestDto> >("application/json");
            info->addResponse<Object<BaseResponseDto> >(Status::CODE_200, "application/json");
        }

        ENDPOINT("POST", "/dataset/{dataset_id}/export", exportDataset, AUTH_HEADER,
                 BODY_DTO(Object<request::ExportDatasetRequestDto>, dto), PATH(Int32, dataset_id)) {
            AUTH
            const auto resp = SimpleDataResponseDto<String>::createShared();
            const auto dataset = dataset::ImageDataset::getById<dataset::ImageDataset>(dataset_id);
            OATPP_ASSERT_HTTP(dataset != nullptr, Status::CODE_404, "Dataset does not exist.")
            OATPP_ASSERT_HTTP(USER->hasPerm("DATASET_READ_["+std::to_string(dataset_id)+"]"), Status::CODE_200,
                              "Permission denied.")
        }

        ENDPOINT_INFO(exportDataset) {
            info->description = "导出数据集";
            info->addResponse<Object<BaseResponseDto> >(Status::CODE_200, "application/json");
        }

        ENDPOINT("GET", "/dataset/rm/{id}", rmDataset, AUTH_HEADER, PATH(Int32, id)) {
            AUTH
            const auto dataset = dataset::ImageDataset::getById(id);
            OATPP_ASSERT_HTTP(dataset != nullptr, Status::CODE_404, "Dataset not found.")
            OATPP_ASSERT_HTTP((USER->hasPerm("DATASET_RM") and dataset->getDto()->owner_id == USER->getId()) or
                              USER->hasPerm("DATASET_RM_[" + std::to_string(id) + "]"),
                              Status::CODE_403, "Permission denied.")
            return createDtoResponse(Status::CODE_200, BaseResponseDto::createShared());
        }

        ENDPOINT("GET", "/dataset/ls", listDataset) {
        }

        ENDPOINT("GET", "/dataset/ch", chDataset) {
        }

        ENDPOINT("GET", "/dataset/info", getDataset) {

        }
    };
}

#include OATPP_CODEGEN_END(ApiController)


#endif //DATASETCONTROLLER_HPP
