//
// Created by ender on 11/26/24.
//

#ifndef DATASETDTO_HPP
#define DATASETDTO_HPP
#include "annotation/AnnotationClassDto.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace ender_label::dto::data {
    class ImageDto : public oatpp::DTO {
        DTO_INIT(ImageDto, DTO)

        DTO_FIELD(Int64, id);

        DTO_FIELD(String, relative_path);

        DTO_FIELD_INFO(relative_path) {
            info->description = "相对路径";
        }

        // DTO_FIELD(String, format);
        //
        // DTO_FIELD_INFO(format) {
        //     info->description = "文件格式";
        // }

        DTO_FIELD(Int64, size);

        DTO_FIELD_INFO(size) {
            info->description = "文件大小，单位字节Byte";
        }

        DTO_FIELD(Int32, width);

        DTO_FIELD_INFO(width) {
            info->description = "宽度，像素";
        }

        DTO_FIELD(Int32, height);

        DTO_FIELD_INFO(height) {
            info->description = "高度，像素";
        }

        DTO_FIELD(String, md5_hash_32);

        DTO_FIELD_INFO(md5_hash_32) {
            info->description = "md5哈希校验值，32位";
        }
    };

    class ImageWithTaskStateDto final : public ImageDto {
        DTO_INIT(ImageWithTaskStateDto, ImageDto)

        DTO_FIELD(Boolean, task_anno_state);

        DTO_FIELD_INFO(task_anno_state) {
            info->description = "是否已经在该任务中标注过";
        }
    };

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

        DTO_FIELD(UnorderedSet<Int64>, img_ids);
        DTO_FIELD_INFO(img_ids) {
            info->description = "包含的图片文件列表";
        }

        DTO_FIELD(List<Int32>, class_ids);
        DTO_FIELD_INFO(class_ids) {
            info->description = "数据集使用的类别id列表";
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
