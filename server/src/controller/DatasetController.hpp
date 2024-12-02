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
        ENDPOINT("POST", "/dataset/add", addDataset, BODY_DTO(Object<data::DatasetDto>, dto), AUTH_HEADER) {
            AUTH
            REQUEST_PARAM_CHECK(dto->class_ids)
            REQUEST_PARAM_CHECK(dto->desc)
            REQUEST_PARAM_CHECK(dto->name)
            OATPP_ASSERT_HTTP(USER->hasPerm("DATASET_ADD"), Status::CODE_403, "Permission denied.")
            const auto resp = SimpleDataResponseDto<Object<data::DatasetDto> >::createShared();
            dto->owner_id = USER->getId();
            const auto dataset = dataset::ImageDataset::createShared(dto);
            dataset->write();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT("POST", "/dataset/{dataset_id}/import", importDataset, AUTH_HEADER,
                 PATH(Int32, dataset_id),
                 BODY_DTO(Object<request::ImportDatasetRequestDto>, dto)) {
            AUTH
            const auto resp = BaseResponseDto::createShared();
            const auto dataset = dataset::ImageDataset::getById<dataset::ImageDataset>(dataset_id);
            OATPP_ASSERT_HTTP(dataset != nullptr, Status::CODE_404, "Dataset does not exist.")
            OATPP_ASSERT_HTTP(USER->hasPerm("DATASET_UPDATE_["+std::to_string(dataset_id)+"]"), Status::CODE_200,
                              "Permission denied.")
            auto img_exts = std::unordered_set<std::string>{};
            auto anno_exts = std::unordered_set<std::string>{};
            std::for_each(dto->img_exts->begin(), dto->img_exts->end(), [&img_exts](auto &x) {
                OATPP_ASSERT_HTTP(x->size() > 1 and x->front() == '.', Status::CODE_400, "Wrong ext format.")
                img_exts.emplace(x);
            });
            std::for_each(dto->anno_exts->begin(), dto->anno_exts->end(), [&anno_exts](auto &x) {
                OATPP_ASSERT_HTTP(x->size() > 1 and x->front() == '.', Status::CODE_400, "Wrong ext format.")
                anno_exts.emplace(x);
            });
            dataset->importYolo(dto->import_dir, img_exts, anno_exts, "segmentation");
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(importDataset) {
            info->description = "导入数据库";
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
