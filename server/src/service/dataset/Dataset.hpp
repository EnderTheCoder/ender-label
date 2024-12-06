//
// Created by ender on 12/2/24.
//

#ifndef DATASET_HPP
#define DATASET_HPP


namespace ender_label::service::dataset {
    class Dataset {
    public:
        virtual ~Dataset() = default;

        virtual void exportVanilla() = 0;

        virtual void importVanilla() = 0;


    };
}


#endif //DATASET_HPP
