//
// Created by ender on 11/26/24.
//

#ifndef DATASETCONTROLLER_HPP
#define DATASETCONTROLLER_HPP
#include <dto/response/ArrayResponseDto.hpp>

#include "dto/request/ImportDatasetRequestDto.hpp"

#include <dto/response/SimpleDataResponseDto.hpp>

#include "service/dataset/ImageDataset.hpp"
#include "service/dataset/annotation/SegmentationAnnotation.hpp"
#include "service/dataset/annotation/AnnotationMerger.hpp"
#include "util/AuthUtil.hpp"
#include "util/SwaggerUtil.hpp"
#include "util/ControllerUtil.hpp"
#include <ranges>
#include <service/processor/BackgroundImageProcessor.hpp>

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
            REQUEST_PARAM_CHECK(dto->desc)
            REQUEST_PARAM_CHECK(dto->name)
            OATPP_ASSERT_HTTP(USER->hasPerm("DATASET_CREATE"), Status::CODE_403, "Permission denied.")
            const auto resp = SimpleDataResponseDto<Object<data::ImageDatasetDto> >::createShared();
            dto->owner_id = USER->getId();
            const auto dataset = dataset::ImageDataset::createShared<dataset::ImageDataset>(dto);
            dataset->write();
            dataset->initStorage();
            const auto root_perm = user::Permission::root();
            for (const auto keys = dataset->getPermKeys(); const auto &key: keys) {
                const auto perm_dto = data::PermissionDto::createShared();
                perm_dto->key = key;
                const auto perm = user::Permission::createShared<user::Permission>(perm_dto);
                perm->write();
                perm->updateParent(root_perm->getId());
                USER->addPerm(key);
            }
            resp->data = dataset->getDto();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(createDataset) {
            info->name = "创建数据集";
            info->description =
                    "创建数据集需要具备权限DATASET_CREATE。\n"
                    "创建一个空的数据集，所有权归当前登录用户所有。\n"
                    + util::swaggerRequiredFields<data::ImageDatasetDto>("必填字段") + "\n" +
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
            OATPP_ASSERT_HTTP(USER->hasPerm("DATASET_UPDATE_["+std::to_string(dataset_id)+"]"), Status::CODE_403,
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
            dataset->importYolo(dto->import_dir, img_exts, anno_exts, "segment");
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(importDataset) {
            info->description =
                    "向一个已经存在的数据集导入数据。\n"
                    "需要确保目录具有访问权限。\n"
                    "需要确保目录中包含的图片和标注文件不含有重复文件名。\n"
                    "当前仅支持从服务器本地路径进行导入。\n"
                    + util::swaggerRequiredFields<request::ImportDatasetRequestDto>("必填字段") + "\n";
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

        ENDPOINT("GET", "/dataset/{dataset_id}/delete", rmDataset, AUTH_HEADER, PATH(Int32, dataset_id)) {
            AUTH
            const auto dataset = dataset::ImageDataset::getById(dataset_id);
            OATPP_ASSERT_HTTP(dataset != nullptr, Status::CODE_404, "Dataset not found.")
            OATPP_ASSERT_HTTP(USER->hasPerm("DATASET_DELETE_[" + std::to_string(dataset_id) + "]"), Status::CODE_403,
                              "Permission denied.")
            dataset->del();
            return createDtoResponse(Status::CODE_200, BaseResponseDto::createShared());
        }

        ENDPOINT_INFO(rmDataset) {
            info->name = "删除数据集";
            info->description = "需要具备DATASET_DELETE权限来删除。";
            info->addResponse<Object<BaseResponseDto> >(Status::CODE_200, "application/json");
        }

        ENDPOINT("GET", "/dataset/all", listAllDataset, AUTH_HEADER) {
            AUTH
            OATPP_ASSERT_HTTP(USER->hasPerm("DATASET_LIST"), Status::CODE_403, "Permission denied.")
            const auto resp = ArrayResponseDto<Object<data::SizedImageDatasetDto> >::createShared();
            const auto dtos = dataset::ImageDataset::toDtoList(dataset::ImageDataset::getAll());
            resp->data = {};
            resp->data->resize(dtos->size(), nullptr);
            for (auto it = dtos->begin(); it != dtos->end(); ++it) {
                const auto sized_dto = data::SizedImageDatasetDto::createShared();
                const auto dto = *it;
                if (dto->img_ids == nullptr) {
                    sized_dto->img_size = 0;
                } else {
                    sized_dto->img_size = static_cast<int>(dto->img_ids->size());
                }
                // TODO: load anno size
                sized_dto->anno_size = 0;
                dto->img_ids = nullptr;
                util::Util::copyToUpper(dto, sized_dto);
                resp->data->at(it - dtos->begin()) = sized_dto;
            }
            resp->size = resp->data->size();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(listAllDataset) {
            info->name = "列出所有数据集";
            info->description = "需要权限DATASET_LIST。";
            info->addResponse<Object<ArrayResponseDto<Object<data::ImageDatasetDto> > > >(
                Status::CODE_200, "application/json");
        }

        ENDPOINT("GET", "/dataset/ch", chDataset) {
        }

        ENDPOINT("GET", "/dataset/info", getDataset) {
        }

        ENDPOINT("GET", "/dataset/{dataset_id}/image/list", listImage, QUERY(Int32,page), QUERY(Int32, size)) {
        }

        ENDPOINT_INFO(listImage) {
            info->description = "分页查询指定数据集下面的图片";
        }

        ENDPOINT("GET", "/dataset/{dataset_id}/image/{image_id}/annotation/all", listImageAnnotation,
                 QUERY(String, task)) {
        }

        ENDPOINT_INFO(listImageAnnotation) {
            info->description = "列出指定图片下所有标注\n"
                    "task是可选参数，如果不指定默认为所有标注。";
        }

        ENDPOINT("POST", "/dataset/{dataset_id}/annotation/save", saveAnnotation) {

        }

        ENDPOINT_INFO(saveAnnotation) {
            info->description =
                    "保存标注，如果不存在则创建，如果存在则覆盖。\n";
        }

        ENDPOINT("GET", "/dataset/image/{image_id}/thumbnail", getThumbnail, PATH(Int64, image_id), AUTH_HEADER) {
            AUTH
            const auto img = dataset::Image::getById<dataset::Image>(image_id);
            OATPP_ASSERT_HTTP(img != nullptr, Status::CODE_404, "Requested image does not exist.")
            const auto thumbnail_path = processor::BackgroundImageProcessor::imgThumbnailPath(
                img->getDto()->relative_path);
            auto mat = cv::Mat{};
            if (exists(thumbnail_path)) {
                mat = cv::imread(thumbnail_path);
            } else {
                mat = img->readCvImgFromDisk();
            }
            std::vector<uchar> buffer;
            imencode(".png", mat, buffer);
            auto resp = createResponse(Status::CODE_200, std::string(buffer.begin(), buffer.end()));
            resp->putHeader("Content-Type", "image/png");
            return resp;
        }

        ENDPOINT_INFO(getThumbnail) {
            info->description = "获取指定图片的缩略图，如果不存在返回原图";
            info->addResponse<String>(Status::CODE_200, "image/png");
        }
    };
}

#include OATPP_CODEGEN_END(ApiController)


#endif //DATASETCONTROLLER_HPP
