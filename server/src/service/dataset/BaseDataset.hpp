//
// Created by ender on 11/26/24.
//

#ifndef BASEDATASET_HPP
#define BASEDATASET_HPP

#include "service/ServiceBean.hpp"
#include "dto/data/DatasetDto.hpp"

namespace ender_label::service::dataset {
    constexpr char table_name[] = "dataset";

    class BaseDataset : public ServiceBean<table_name, DatasetDto> {
    public:
    };
}

#endif //BASEDATASET_HPP
