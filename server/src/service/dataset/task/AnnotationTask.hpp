//
// Created by ender on 12/11/24.
//

#ifndef ANNOTATIONTASK_HPP
#define ANNOTATIONTASK_HPP


#include "service/ServiceBean.hpp"
#include "dto/data/task/AnnotationTaskDto.hpp"
#include "dto/data/task/QuantityTaskDto.hpp"
#include "dto/data/task/DesignatedImageTaskDto.hpp"

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

        static auto makeLog(const auto &img_id, const auto &anno_id, const auto &user_id, const auto &log_type) {
            const auto log_dto = AnnotationLogDto::createShared();
            log_dto->img_id = img_id;
            log_dto->anno_id = anno_id;
            log_dto->user_id = user_id;
            log_dto->time = util::TimeUtil::getCurrentTimestampInLong();
            log_dto->log_type = log_type;
            return log_dto;
        }

        auto log(auto &log_dto) {
            const auto task_dto = readTaskDto();
            task_dto->logs->push_back(log_dto);
            writeTaskDto(task_dto);
        }

        //create read update delete
        virtual void onReadAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) {
            if (this->readTaskDto()->config->match_read) {
                const auto log = makeLog(anno_dto->img_id, anno_dto->id, anno_dto->owner_id, AnnoLogType::READ);
                this->log(log);
            }
        }

        virtual void onUpdateAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) {
            if (this->readTaskDto()->config->match_update) {
                const auto log = makeLog(anno_dto->img_id, anno_dto->id, anno_dto->owner_id, AnnoLogType::UPDATE);
                this->log(log);
            }
        }

        virtual void onCreateAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) {
            if (this->readTaskDto()->config->match_create) {
                const auto log = makeLog(anno_dto->img_id, anno_dto->id, anno_dto->owner_id, AnnoLogType::CREATE);
                this->log(log);
            }
        }

        virtual void onDeleteAnno(const oatpp::Object<annotation::AnnotationDto> &anno_dto) {
            if (this->readTaskDto()->config->match_delete) {
                const auto log = makeLog(anno_dto->img_id, anno_dto->id, anno_dto->owner_id, AnnoLogType::DELETE);
                this->log(log);
            }
        }

        virtual bool relevantWithUser(const Int32 &uid) {
            return this->getDto()->user_ids == nullptr or this->getDto()->user_ids->contains(uid);
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
        }

        void decrease() {
        }
    };

    class DesignatedImageTask final : public AnnotationTask<DesignatedImageTaskDto> {
    public:
    };
}


#endif //ANNOTATIONTASK_HPP
