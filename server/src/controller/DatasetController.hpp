//
// Created by ender on 11/26/24.
//

#ifndef DATASETCONTROLLER_HPP
#define DATASETCONTROLLER_HPP
#include <dto/response/SimpleDataResponseDto.hpp>

#include "service/dataset/BaseDataset.hpp"
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
        ENDPOINT("GET", "/dataset/add", addDataset, BODY_DTO(Object<data::DatasetDto>, dto), AUTH_HEADER) {
            AUTH
            REQUEST_PARAM_CHECK(dto->class_ids)
            REQUEST_PARAM_CHECK(dto->desc)
            REQUEST_PARAM_CHECK(dto->name)

            if (!USER->hasPerm("DATASET_ADD")) {
                ERROR(Status::CODE_403, "Permission denied.")
            }
            const auto resp = SimpleDataResponseDto<Object<data::DatasetDto> >::createShared();
            dto->owner_id = USER->getId();
            const auto dataset = dataset::BaseDataset::createShared(dto);
            dataset->write();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT("GET", "/dataset/import", importDataset) {
        }

        ENDPOINT("GET", "/dataset/export", exportDataset) {
        }

        ENDPOINT("GET", "/dataset/rm/{id}", rmDataset, AUTH_HEADER, PATH(Int32, id)) {
            AUTH
            const auto dataset = dataset::BaseDataset::getById(id);
            OATPP_ASSERT(dataset != nullptr, Status::CODE_404, "Dataset not found.")
            OATPP_ASSERT((USER->hasPerm("DATASET_RM") and dataset->getDto()->owner_id == USER->getId()) or
                         USER->hasPerm("ROOT") or
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
