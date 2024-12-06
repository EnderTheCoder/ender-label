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

        virtual int getDatasetId() = 0;

        virtual std::string getUpdatePermKey() {
            return "DATASET_UPDATE_[" + std::to_string(this->getDatasetId()) + "]";
        }

        virtual std::string getDeletePermKey() {
            return "DATASET_DELETE_[" + std::to_string(this->getDatasetId()) + "]";
        }

        virtual std::string getReadPermKey() {
            return "DATASET_READ_[" + std::to_string(this->getDatasetId()) + "]";
        }

        virtual std::vector<std::string> getPermKeys() {
            return {getUpdatePermKey(), getDeletePermKey(), getReadPermKey()};
        }
    };
}


#endif //DATASET_HPP
