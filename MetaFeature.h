#ifndef _META_FEATURE_H_
#define _META_FEATURE_H_

#include "MetaData.h"

namespace feature_extraction
{
//flag = 0: not initiated
struct MetaFeatureInfo
{
    const char * feature_name;

    const uint32_t * meta_feature_list;
    uint32_t meta_feature_num;

    uint32_t feature_index;
    bool is_init;
};

class MetaFeature
{
public:
    MetaFeature() {feature_info_ptr_ = NULL;};
    virtual ~MetaFeature() {};
    virtual int get_feature(const MetaData & meta, const uint32_t dst_num_limit, void * dst_ptr, uint32_t * dst_num) const = 0;
    virtual int init(MetaData & meta) = 0;
    MetaFeatureInfo * get_feature_info() {return feature_info_ptr_;};
protected:
    MetaFeatureInfo * feature_info_ptr_;
};
}
#endif
