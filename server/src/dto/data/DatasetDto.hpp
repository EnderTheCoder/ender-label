//
// Created by ender on 11/26/24.
//

#ifndef DATASETDTO_HPP
#define DATASETDTO_HPP
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>
#include <oatpp/codegen/dto/base_define.hpp>

#include "annotation/AnnotationClassDto.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data {
    class ImageDatasetDto : public oatpp::DTO {
        DTO_INIT(ImageDatasetDto, DTO)
        DTO_FIELD(Int32, id);
        DTO_FIELD(Int32, owner_id);
        DTO_FIELD_INFO(owner_id) {
            info->description = "所有者id";
        }

        DTO_FIELD(String, name);
        DTO_FIELD_INFO(name) {
            info->description = "数据集名称";
            info->required = true;
        }

        DTO_FIELD(String, desc);
        DTO_FIELD_INFO(desc) {
            info->description = "描述";
            info->required = true;
        }

        DTO_FIELD(UnorderedSet<String>, img_files);
        DTO_FIELD_INFO(img_files) {
            info->description = "包含的图片文件列表";
        }

        DTO_FIELD(List<Int32>, class_ids);
        DTO_FIELD_INFO(class_ids) {
            info->description = "数据集使用的类别id列表";
            info->required = true;
        }
    };

    class SizedImageDatasetDto final : public ImageDatasetDto {
        DTO_INIT(SizedImageDatasetDto, ImageDatasetDto)
        DTO_FIELD(Int32, img_size);
        DTO_FIELD_INFO(img_size) {
            info->description = "图片数量";
        }

        DTO_FIELD(Int32, anno_size);
        DTO_FIELD_INFO(anno_size) {
            info->description = "标注数量";
        }

        DTO_FIELD(String, type) = "image";
        DTO_FIELD_INFO(type) {
            info->description = "数据集类型";
        }
    };
}

#include OATPP_CODEGEN_END(DTO)


#endif //DATASETDTO_HPP
