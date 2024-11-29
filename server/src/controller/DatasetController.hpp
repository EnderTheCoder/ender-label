//
// Created by ender on 11/26/24.
//

#ifndef DATASETCONTROLLER_HPP
#define DATASETCONTROLLER_HPP
#include "service/dataset/BaseDataset.hpp"
#include "service/dataset/annotation/SegmentationAnnotation.hpp"
#include "service/dataset/annotation/AnnotationMerger.hpp"
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
        ENDPOINT("GET", "/dataset/add", addDataset) {
        }

        ENDPOINT("GET", "/dataset/rm", rmDataset) {
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
