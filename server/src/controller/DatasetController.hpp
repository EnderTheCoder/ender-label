//
// Created by ender on 11/26/24.
//

#ifndef DATASETCONTROLLER_HPP
#define DATASETCONTROLLER_HPP
#include <dto/response/ArrayResponseDto.hpp>

#include "dto/request/ImportDatasetRequestDto.hpp"

#include <dto/response/SimpleDataResponseDto.hpp>
#include "dto/response/PagedResponseDto.hpp"
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
            dataset->importYolo(dto->import_dir, img_exts, anno_exts, TaskType::segment);
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
                //todo: anno size
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

        // ENDPOINT("GET", "/dataset/{dataset_id}/image/list", listImage, QUERY(Int32,page), QUERY(Int32, size),
        //          AUTH_HEADER) {
        //     AUTH
        //     OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
        //     const auto resp = PagedResponseDto<Object<data::ImageDto> >::createShared();
        //     resp->data = dataset::Image::toDtoList(dataset::Image::page(size, page));
        //     resp->page_num = page;
        //     resp->page_size = size;
        //     resp->page_total = dataset::Image::count(
        //         db->executeQuery("SELECT count(id) FROM ender_label_img WHERE id in"));
        //     return createDtoResponse(Status::CODE_200, resp);
        // }
        //
        // ENDPOINT_INFO(listImage) {
        //     info->description = "分页查询指定数据集下面的图片信息（不包含图片数据，只有图片各项基本信息）";
        // }

        ENDPOINT("GET", "/dataset/{dataset_id}/image/all", getAllImage, PATH(Int32,dataset_id), AUTH_HEADER) {
            AUTH
            const auto dataset = dataset::ImageDataset::getById(dataset_id);
            OATPP_ASSERT_HTTP(dataset != nullptr, Status::CODE_404, "Requested dataset not found")
            const auto resp = ArrayResponseDto<Object<data::ImageDto> >::createShared();
            resp->data = {};
            if (dataset->getDto()->img_ids != nullptr) {
                for (const auto &img_id: *dataset->getDto()->img_ids) {
                    const auto img = dataset::Image::getById(img_id);
                    if (img == nullptr) {
                        OATPP_LOGE("DATASET", "Img with id %lld not found.", *img_id);
                    }
                    resp->data->emplace_back(img->getDto());
                }
            }
            resp->size = resp->data->size();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(getAllImage) {
            info->name = "获取数据集图片基本信息";
            info->description = "查询指定数据集下面所有的图片信息（不包含图片数据，只有图片各项基本信息）";
            info->addResponse<Object<ArrayResponseDto<Object<
                data::ImageDto> > > >(Status::CODE_200, "application/json");
        }

        ENDPOINT("GET", "/dataset/{dataset_id}/image/{image_id}/annotation/all", listImageAnnotation,
                 QUERY(Enum<TaskType>::AsString, task), PATH(Int32, dataset_id), PATH(Int64, image_id)) {
            const auto dataset = dataset::ImageDataset::getById(dataset_id);
            const auto image = dataset::Image::getById(image_id);
            OATPP_ASSERT_HTTP(dataset != nullptr, Status::CODE_200, "Requested dataset does not exist.")
            OATPP_ASSERT_HTTP(image != nullptr, Status::CODE_200, "Requested image does not exist.")
            if (dataset->getDto()->img_ids != nullptr) {
                for (const auto &img_id: *dataset->getDto()->img_ids) {
                    if (img_id == image->getId()) { goto image_in_dataset_ok; }
                }
            }
            throw oatpp::web::protocol::http::HttpError(Status::CODE_400,
                                                        "The image[id:" + std::to_string(*image_id) +
                                                        "] is not in the dataset.");
        image_in_dataset_ok:
            auto resp = ArrayResponseDto<oatpp::Object<data::AnnotationDto> >::createShared();
            using namespace dataset::annotation;
            resp->data = {};
            for (const auto &anno_dto: *Annotation::toDtoList(Annotation::getByField("img_id", image_id)) |
                                       std::views::filter([&task](auto &x) {
                                           return x->task_type == task;
                                       })) {
                resp->data->push_back(anno_dto);
            }
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(listImageAnnotation) {
            info->name = "获取图片的所有标注";
            info->description = "列出指定图片下所有标注\n"
                    "task是可选参数，如果不指定默认为所有标注。";
            info->addResponse<Object<ArrayResponseDto<oatpp::Object<data::AnnotationDto> > > >(
                Status::CODE_200, "application/json");
        }

        ENDPOINT("GET", "/dataset/{dataset_id}/annotation/class/all", getDatasetAnnoClass, PATH(Int32, dataset_id),
                 AUTH_HEADER) {
            AUTH
            auto dataset = dataset::ImageDataset::getById<dataset::ImageDataset>(dataset_id);
            OATPP_ASSERT_HTTP(dataset != nullptr, Status::CODE_404, "Requested dataset not found.")
            const auto resp = ArrayResponseDto<Object<data::annotation::AnnotationClassDto> >::createShared();
            resp->data = dataset->getClasses();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(getDatasetAnnoClass) {
            info->name = "获取数据集标注类别映射表";
            info->description = "返回指定数据集标注使用的类别的映射表";
            info->addResponse<Object<ArrayResponseDto<Object<data::annotation::AnnotationClassDto> > > >(
                Status::CODE_200, "application/json");
        }

        ENDPOINT("POST", "/dataset/{dataset_id}/annotation/save", saveAnnotation,
                 BODY_DTO(Object<data::AnnotationDto>, req), AUTH_HEADER, PATH(Int32, dataset_id)) {
            AUTH
            REQUEST_PARAM_CHECK(req->img_id)
            REQUEST_PARAM_CHECK(req->anno_cls_ids)
            REQUEST_PARAM_CHECK(req->raw_json)
            REQUEST_PARAM_CHECK(req->task_type)
            const auto dataset = dataset::ImageDataset::getById(dataset_id);
            OATPP_ASSERT_HTTP(dataset!= nullptr, Status::CODE_404,
                              "Requested dataset[id:"+std::to_string(*dataset_id)+"] not found.")
            OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, mapper);
            try {
                switch (*req->task_type) {
                    case TaskType::detect: {
                        const auto detect_dto = mapper->readFromString<Object<data::annotation::ObjectDetectionDto> >(
                            req->raw_json);
                        OATPP_ASSERT_HTTP(detect_dto->bboxes != nullptr, Status::CODE_400, "Field bboxes cannot be null, use empty list instead.")
                        for (const auto &bbox: *detect_dto->bboxes) {
                            OATPP_ASSERT_HTTP(bbox->cls_id != nullptr, Status::CODE_400,
                                              "Bbox cls_id field must not be null.")
                            OATPP_ASSERT_HTTP(
                                std::ranges::any_of(req->anno_cls_ids->begin(), req->anno_cls_ids->end(),
                                    [&bbox](auto& x) {
                                    return x != nullptr and x == bbox->cls_id;
                                    }), Status::CODE_400,
                                "Annotation class id used in bboxes must be included in json->anno_cls_ids field too.")
                            OATPP_ASSERT_HTTP(bbox->normalized_xyxy != nullptr, Status::CODE_400,
                                              "Bbox normalized_xyxy field must not be null.")
                            OATPP_ASSERT_HTTP(bbox->normalized_xyxy->size() == 4, Status::CODE_400,
                                              "Bbox normalized_xyxy must contains 4 points.")
                            OATPP_ASSERT_HTTP(
                                std::ranges::none_of(bbox->normalized_xyxy->begin(), bbox->normalized_xyxy->end(),
                                    [](auto& x) {
                                    return x>1 or x<0;
                                    }), Status::CODE_400,
                                "Bbox normalized_xyxy val must be in range [0, 1].")
                        }
                        break;
                    }
                    case TaskType::segment: {
                        const auto segment_dto = mapper->readFromString<Object<data::annotation::SegmentationDto> >(
                            req->raw_json);
                        OATPP_ASSERT_HTTP(segment_dto->polygons != nullptr, Status::CODE_400, "Field polygons cannot be null, use empty list instead.")
                        OATPP_ASSERT_HTTP(segment_dto->masks != nullptr, Status::CODE_400, "Field masks cannot be null, use empty list instead.")
                        for (const auto &polygon: *segment_dto->polygons) {
                            OATPP_ASSERT_HTTP(polygon->cls_id != nullptr, Status::CODE_400,
                                              "Polygon cls_id field must not be null.")
                            OATPP_ASSERT_HTTP(polygon->normalized_points != nullptr, Status::CODE_400,
                                              "Polygon normalized_points field must not be null.")
                            OATPP_ASSERT_HTTP(polygon->normalized_points->size()>= 3, Status::CODE_400,
                                              "There should be at least 3 points in a polygon.")
                            OATPP_ASSERT_HTTP(
                                std::ranges::any_of(req->anno_cls_ids->begin(), req->anno_cls_ids->end(),
                                    [&polygon](auto& x) {
                                    return x != nullptr and x == polygon->cls_id;
                                    }), Status::CODE_400,
                                "Annotation class id used in polygons must be included in json->anno_cls_ids field too.")
                            OATPP_ASSERT_HTTP(
                                std::ranges::none_of(polygon->normalized_points->begin(), polygon->normalized_points
                                    ->
                                    end(),
                                    [](auto& x) {
                                    return x == nullptr or x->size() !=2 or x[0]==nullptr or x[1]==nullptr or x[0] <
                                    0
                                    or x[
                                        0] > 1 or x[1] < 0 or x[1] >1;
                                    }), Status::CODE_400, "Exceed normalized points axis range limit [0, 1]")
                        }
                        break;
                    }
                    case TaskType::pose: {
                        const auto pose_dto = mapper->readFromString<Object<data::annotation::PoseDto> >(req->raw_json);
                        OATPP_ASSERT_HTTP(pose_dto->points != nullptr, Status::CODE_400, "Field points cannot be null, use empty list instead.")
                        for (const auto &point: *pose_dto->points) {
                            OATPP_ASSERT_HTTP(point->cls_id != nullptr, Status::CODE_400,
                                              "Point cls_id field must not be null.")
                            OATPP_ASSERT_HTTP(
                                std::ranges::any_of(req->anno_cls_ids->begin(), req->anno_cls_ids->end(),
                                    [&point](auto& x) {
                                    return x != nullptr and x == point->cls_id;
                                    }), Status::CODE_400,
                                "Annotation class id used in points must be included in json->anno_cls_ids field too.")
                            OATPP_ASSERT_HTTP(
                                point->normalized_x != nullptr and
                                point->normalized_y != nullptr and
                                point->normalized_x > 0 and
                                point->normalized_x < 1 and
                                point->normalized_y > 0 and
                                point->normalized_y < 1,
                                Status::CODE_400, "Exceed normalized points axis range limit [0, 1]")
                        }
                        break;
                    }
                }
            } catch (oatpp::parser::ParsingError &e) {
                throw oatpp::web::protocol::http::HttpError(
                    Status::CODE_400,
                    "Unparsable wrong json format for field 'raw_json'.");
            }
            req->owner_id = USER->getId();
            const auto img = dataset::Image::getById(req->img_id);
            OATPP_ASSERT_HTTP(img != nullptr, Status::CODE_404, "Requested image not found.")
            for (const auto &anno_cls_id: *req->anno_cls_ids) {
                OATPP_ASSERT_HTTP(dataset::annotation::AnnotationClass::getById(anno_cls_id) != nullptr,
                                  Status::CODE_404, "Requested annotation class not found.")
            }
            std::shared_ptr<dataset::annotation::Annotation> anno = nullptr;
            if (req->id == nullptr) {
                anno = dataset::annotation::Annotation::createShared<dataset::annotation::Annotation>(req);
                anno->write();
            } else {
                anno = dataset::annotation::Annotation::getById<dataset::annotation::Annotation>(req->id);
                OATPP_ASSERT_HTTP(anno != nullptr, Status::CODE_404,
                                  "Requested annotation[id:"+std::to_string(*req->id)+"] not found")
                anno->overwrite(req, {"raw_json", "anno_cls_ids"});
            }

            const auto resp = SimpleDataResponseDto<Object<data::AnnotationDto> >::createShared();
            resp->data = anno->getDto();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(saveAnnotation) {
            info->description = "保存标注，如果不存在则创建，如果存在则覆盖。\n" +
                                util::swaggerRequiredFields<data::AnnotationDto>() + "\n" +
                                "如果填写id字段将覆盖源有标注，如果不填写id字段则创建新标注。\n"
                                "如果是覆盖原有的标注，只填写id(必填), raw_json(选填), anno_cls_ids(选填)这三个字段。";
        }

        ENDPOINT("GET", "/dataset/image/{image_id}/thumbnail", getThumbnail, PATH(Int64, image_id)) {
            const auto img = dataset::Image::getById<dataset::Image>(image_id);
            OATPP_ASSERT_HTTP(img != nullptr, Status::CODE_404, "Requested image does not exist.")
            const auto thumbnail_path = processor::BackgroundImageProcessor::imgThumbnailPath(
                img->getDto()->md5_hash_32);
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
            info->description = "获取指定图片的缩略图，如果不存在返回原图。格式为png，不需要传入token。";
            info->addResponse<String>(Status::CODE_200, "image/png");
        }

        ENDPOINT("GET", "/dataset/image/{image_id}/source", getImage, PATH(Int64, image_id)) {
            const auto img = dataset::Image::getById<dataset::Image>(image_id);
            OATPP_ASSERT_HTTP(img != nullptr, Status::CODE_404, "Requested image does not exist.")
            auto mat = img->readCvImgFromDisk();
            std::vector<uchar> buffer;
            imencode(".png", mat, buffer);
            auto resp = createResponse(Status::CODE_200, std::string(buffer.begin(), buffer.end()));
            resp->putHeader("Content-Type", "image/png");
            return resp;
        }

        ENDPOINT_INFO(getImage) {
            info->description = "获取指定图片的原始图，格式为png，不需要传入token。";
            info->addResponse<String>(Status::CODE_200, "image/png");
        }
    };
}

#include OATPP_CODEGEN_END(ApiController)


#endif //DATASETCONTROLLER_HPP
