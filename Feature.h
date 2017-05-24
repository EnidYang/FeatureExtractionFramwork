#ifndef _FEATURE_H_
#define _FEATURE_H_

#include "MetaFeature.h"

namespace feature_extraction
{
typedef std::vector<uint32_t> FeatureVector;
struct FeatureID
{
     uint32_t type_id;
     uint64_t local_id;
}

class Feature
{
protected:
    FeatureVector feature_list_;
public:
    Feature() {};
    virtual ~Feature() {};
    virtual int get_feature(const MetaFeature & meta_feature,
                            const MetaData & meta_data,
                            const uint32_t feature_num_limit,
                            FeatureID * feature_list,
                            uint32_t * feature_num
                           ) const = 0;

    const FeatureVector & get_feature_list() {return feature_list_;};
};

}
#endif
