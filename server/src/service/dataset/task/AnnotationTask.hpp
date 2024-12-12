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
            const auto log_dto = AnnotationLogDto::createShared();
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
        virtual void onReadAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) {
        }

        virtual void onUpdateAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) {
        }

        virtual void onCreateAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) {
        }

        virtual void onDeleteAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) {
        }

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

        auto getLogsWithImgAndType(const auto img_id, const auto &type) {
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

        void finish() {
            const auto dto = AnnotationTaskDto::createShared();
            dto->state = true;
            this->overwrite(dto);
        }
    };

    class QuantityTask final : public AnnotationTask<QuantityTaskDto> {
    public:
        void increase() {
            const auto task_data_dto = this->readTaskDto();
            task_data_dto->amount = task_data_dto->amount + 1;
            this->writeTaskDto(task_data_dto);
        }

        void decrease() {
            const auto task_data_dto = this->readTaskDto();
            task_data_dto->amount = task_data_dto->amount - 1;
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
    };


    class DesignatedImageTask final : public AnnotationTask<DesignatedImageTaskDto> {
    public:
        auto getImages() -> Vector<Object<data::ImageDto> > override {
            const auto task_data_dto = this->readTaskDto();
            const auto img_dtos = Vector<Object<data::ImageDto> >::createShared();
            if (task_data_dto->target_img_ids == nullptr) return {};
            for (const auto img_id: *task_data_dto->target_img_ids) {
                const auto img = Image::getById(img_id);
                if (img != nullptr) {
                    img_dtos->push_back(img->getDto());
                }
            }
            return img_dtos;
        }
    };

    typedef AnnotationTask<AnnotationTaskDataDto> BaseTask;
}


#endif //ANNOTATIONTASK_HPP
