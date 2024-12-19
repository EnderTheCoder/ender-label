//
// Created by ender on 12/11/24.
//

#ifndef ANNOTATIONTASK_HPP
#define ANNOTATIONTASK_HPP


#include "service/ServiceBean.hpp"
#include "dto/data/task/AnnotationTaskDto.hpp"
#include "dto/data/task/QuantityTaskDto.hpp"
#include "dto/data/task/DesignatedImageTaskDto.hpp"
#include "dto/data/task/AnnotationLogDto.hpp"
#include "AnnotationLog.hpp"

namespace ender_label::service::dataset::task {
    constexpr char table_name_annotation_task[] = "ender_label_annotation_task";
    using namespace data::task;

    template<typename TASK_DTO_TYPE>
    class AnnotationTask : public ServiceBean<table_name_annotation_task, AnnotationTaskDto> {
    public:
        explicit AnnotationTask(const auto &_dto) : ServiceBean(_dto) {
        }

        auto readTaskDto() {
            const OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, mapper);
            return mapper->readFromString<Object<TASK_DTO_TYPE> >(this->getDto()->raw_json);
        }

        auto writeTaskDto(auto &task_dto) {
            const OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, mapper);
            const auto dto = AnnotationTaskDto::createShared();
            dto->raw_json = mapper->writeToString(task_dto);
            this->overwrite(dto);
        }

        auto makeLog(const auto &img_id, const auto &anno_id, const auto &user_id, const auto &log_type) {
            auto log_dto = AnnotationLogDto::createShared();
            log_dto->img_id = img_id;
            log_dto->anno_id = anno_id;
            log_dto->user_id = user_id;
            log_dto->task_id = this->getId();
            log_dto->time = util::TimeUtil::getCurrentTimestampInLong();
            log_dto->log_type = log_type;
            return log_dto;
        }

        auto log(auto &log_dto) {
            const auto log = AnnotationLog::createShared(log_dto);
            log->write();
        }

        virtual void onAnnoChange(const oatpp::Object<annotation::AnnotationDto> &anno_dto, const AnnoLogType &type) {
            switch (type) {
                case AnnoLogType::READ: {
                    onReadAnno(anno_dto);
                    const auto log = makeLog(anno_dto->img_id, anno_dto->id, anno_dto->owner_id, AnnoLogType::READ);
                    this->log(log);
                    break;
                }
                case AnnoLogType::UPDATE: {
                    onUpdateAnno(anno_dto);
                    const auto log = makeLog(anno_dto->img_id, anno_dto->id, anno_dto->owner_id, AnnoLogType::UPDATE);
                    this->log(log);
                    break;
                }
                case AnnoLogType::CREATE: {
                    onCreateAnno(anno_dto);
                    const auto log = makeLog(anno_dto->img_id, anno_dto->id, anno_dto->owner_id, AnnoLogType::CREATE);
                    this->log(log);
                    break;
                }
                case AnnoLogType::DELETE: {
                    onDeleteAnno(anno_dto);
                    const auto log = makeLog(anno_dto->img_id, anno_dto->id, anno_dto->owner_id, AnnoLogType::DELETE);
                    this->log(log);
                    break;
                }
            }
        }

        //create read update delete
        virtual void onReadAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) = 0;

        virtual void onUpdateAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) = 0;

        virtual void onCreateAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) = 0;

        virtual void onDeleteAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) = 0;

        virtual bool relevantWithUser(const Int32 &uid) {
            return this->getDto()->user_ids == nullptr or this->getDto()->user_ids->contains(uid);
        }

        static auto getUserTasksInDataset(const auto &uid, const auto &dataset_id, bool wrapped = false) {
            const OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
            const auto query = db->executeQuery(
                "SELECT * FROM " + getTableName() +
                " WHERE :user_id = ANY(user_ids) and dataset_id = :dataset_id", {
                    {"user_id", uid},
                    {"dataset_id", dataset_id}
                });
            return getList(query, wrapped);
        }

        static auto getUserTasks(const auto &uid, bool wrapped = false) {
            const OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
            const auto query = db->executeQuery(
                "SELECT * FROM " + getTableName() +
                " WHERE :user_id = ANY(user_ids)", {
                    {"user_id", uid}
                });
            return getList(query, wrapped);
        }

        virtual auto getImageIds() -> Vector<Int64> = 0;

        virtual auto getImages() -> Vector<Object<data::ImageDto> > = 0;

        auto getLogsWithImg(const auto img_id) {
            const OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
            const auto query = db->executeQuery(
                "SELECT * FROM " + AnnotationLog::getTableName() + " WHERE task_id = :task_id, img_id = :img_id", {
                    {"img_id", Int64(img_id)},
                    {"task_id", Int32(this->getId())}
                });
            return AnnotationLog::toDtoList(AnnotationLog::getList(query));
        }

        auto getLogsWithImgAndType(const auto &img_id, const auto &type) {
            const OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
            const auto query = db->executeQuery(
                "SELECT * FROM " + AnnotationLog::getTableName() +
                " WHERE task_id = :task_id and img_id = :img_id and log_type=:log_type", {
                    {"img_id", Int64(img_id)},
                    {"task_id", Int32(this->getId())},
                    {"log_type", Enum<AnnoLogType>(type)}
                });
            return AnnotationLog::toDtoList(AnnotationLog::getList(query));
        }

        void progress(const float percentage) {
            const auto dto = AnnotationTaskDto::createShared();
            dto->progress = percentage;
            this->overwrite(dto);
        }

        void finish() {
            const auto dto = AnnotationTaskDto::createShared();
            dto->state = true;
            this->overwrite(dto);
        }
    };

    class QuantityTask final : public AnnotationTask<QuantityTaskDto> {
    public:
        explicit QuantityTask(const auto &_dto): AnnotationTask(_dto) {
        }

        static auto createSharedR(const Object<AnnotationTaskDto> &dto) {
            return std::make_shared<QuantityTask>(dto);
        }

        auto getImageIds() -> Vector<Int64> override {
            return {};
        }

        auto getImages() -> Vector<Object<data::ImageDto> > override {
            return {};
        }

        void increase() {
            const auto task_data_dto = this->readTaskDto();
            task_data_dto->amount = task_data_dto->amount + 1;
            this->progress(
                static_cast<float>(task_data_dto->amount) / static_cast<float>(task_data_dto->target_amount));
            this->writeTaskDto(task_data_dto);
        }

        void decrease() {
            const auto task_data_dto = this->readTaskDto();
            task_data_dto->amount = task_data_dto->amount - 1;
            this->progress(
                static_cast<float>(task_data_dto->amount) / static_cast<float>(task_data_dto->target_amount));
            this->writeTaskDto(task_data_dto);
        }

        void onCreateAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) override {
            if (this->getLogsWithImgAndType(anno_dto->img_id, AnnoLogType::CREATE)->empty()) increase();
        }

        void onUpdateAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) override {
            if (this->getLogsWithImgAndType(anno_dto->img_id, AnnoLogType::CREATE)->empty() and
                this->getLogsWithImgAndType(anno_dto->img_id, AnnoLogType::UPDATE)->empty())
                increase();
        }

        void onReadAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) override {
        }

        void onDeleteAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) override {
        }
    };


    class DesignatedImageTask final : public AnnotationTask<DesignatedImageTaskDto> {
    public:
        explicit DesignatedImageTask(const auto &_dto): AnnotationTask(_dto) {
        }

        static auto createSharedR(const Object<AnnotationTaskDto> &dto) {
            return std::make_shared<DesignatedImageTask>(dto);
        }

        auto getImageIds() -> Vector<Int64> override {
            const auto task_data_dto = this->readTaskDto();
            const auto ids = Vector<Int64>::createShared();
            if (task_data_dto->target_img_ids != nullptr) {
                std::copy(task_data_dto->target_img_ids->begin(), task_data_dto->target_img_ids->end(), ids->begin());
            }
            return ids;
        }

        auto getImages() -> Vector<Object<data::ImageDto> > override {
            const auto task_data_dto = this->readTaskDto();
            auto img_dtos = Vector<Object<data::ImageDto> >::createShared();
            if (task_data_dto->target_img_ids == nullptr) return {};
            for (const auto &img_id: *task_data_dto->target_img_ids) {
                if (const auto img = Image::getById(img_id); img != nullptr) {
                    img_dtos->push_back(img->getDto());
                }
            }
            return img_dtos;
        }

        void onReadAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) override {
        }

        void onUpdateAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) override {
            const auto task_data_dto = this->readTaskDto();
            if (not task_data_dto->img_ids->contains(anno_dto->img_id) and task_data_dto->target_img_ids->contains(
                    anno_dto->img_id)) {
                task_data_dto->img_ids->emplace(anno_dto->img_id);
                this->writeTaskDto(task_data_dto);
                auto task_dto = AnnotationTaskDto::createShared();
                task_dto->progress = static_cast<float>(task_data_dto->img_ids->size()) / static_cast<float>(
                                         task_data_dto->target_img_ids->size());
                task_dto->state = task_data_dto->img_ids->size() == task_data_dto->target_img_ids->size();
                this->overwrite(task_dto);
            }
        }

        void onCreateAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) override {
            this->onUpdateAnno(anno_dto);
        }

        void onDeleteAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) override {
        }
    };

    typedef AnnotationTask<AnnotationTaskDataDto> BaseTask;
}

#define TRIGGER_TASK_LISTENER(TASK_ID, ANNO, TYPE) \
    if (TASK_ID.getPtr() != nullptr) {\
    const auto _task = ender_label::service::dataset::task::BaseTask::getById<ender_label::service::dataset::task::BaseTask>(TASK_ID); \
    OATPP_ASSERT_HTTP(_task != nullptr, oatpp::web::protocol::http::Status::CODE_404, "Requested task not found.");\
    switch (*_task->getDto()->anno_task_type) { \
    case ender_label::dto::data::task::AnnoTaskType::designated_image: {\
        const auto task = ender_label::service::dataset::task::DesignatedImageTask::createSharedR(_task->getDto());\
task->onAnnoChange(ANNO, TYPE);\
break;\
    }\
    case ender_label::dto::data::task::AnnoTaskType::quantity: {\
        const auto task = ender_label::service::dataset::task::QuantityTask::createSharedR(_task->getDto());\
task->onAnnoChange(ANNO, TYPE);\
break;\
    }\
    default:\
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Error cant get task type.");\
    break;\
                }}
#endif //ANNOTATIONTASK_HPP
