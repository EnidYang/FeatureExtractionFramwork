#ifndef _META_FEATURE_MANAGER_H_
#define _META_FEATURE_MANAGER_H_

#include "FeatureCommon.h"
#include "BaseMetaFeature.h"
#include "MetaFeatureData.h"
#include "DataCenter.h"
#include "RegressionData.h"

static const uint32_t MAX_TERM_LENGTH = 32;

namespace regression
{
//��������ID��ȡ�����ӿڣ�num =0 ��ʾ��������
struct MetaFeatureDataInfo
{
    void * list;
    uint32_t num;
    uint32_t size;
};

//�ǻ�����������Ϣ
struct MetaFeatureInfo
{
    BaseMetaFeature * feature_ptr;
    uint32_t feature_offset;
    uint32_t meta_feature_index;
    uint32_t max_origin_base_feature;
} ;

//���в��������ݣ���ʼ��ʱ��ȡ
struct MetaFeatureAction
{
    std::vector<MetaFeatureInfo> feature_list;
    uint32_t action_flag;
};

class MetaFeatureManager
{
    /*
       MetaFeatureManager��
       metafeaturedata֧��ֻ������
       ���·�ʽΪ��
       1������OneCase
       2����������
       3��������������
       */
public:
    //METAFEATURE����ʹ��Դ��Ϣ���flag
    const static uint32_t METAFEATURE_FLAG_QUERY = 1;
    const static uint32_t METAFEATURE_FLAG_AD = 2;
    const static uint32_t METAFEATURE_FLAG_VISIT_USER = 4;
    const static uint32_t METAFEATURE_FLAG_SHOW = 8;
    const static uint32_t METAFEATURE_FLAG_AD_MATCH = (METAFEATURE_FLAG_QUERY | METAFEATURE_FLAG_AD);
    const static uint32_t METAFEATURE_FLAG_ALL = (METAFEATURE_FLAG_QUERY | METAFEATURE_FLAG_AD_MATCH | METAFEATURE_FLAG_VISIT_USER | METAFEATURE_FLAG_SHOW);

private:
    const static uint32_t FEATURE_INDEX_ORIGIN = 0xFFFFFFFF;
    const static uint32_t MAX_ORIGIN_BASE_FEATURE_ORIGIN = 0xFFFFFFFF;
    const static uint32_t MAX_ORIGIN_BASE_FEATURE_ERROR = 0xFFFFFFFE;


public:
    //    real_copy(const MetaFeature & meta_feature);
    MetaFeatureManager(): is_init_(false), meta_add_error_flag_(false) {}
    ~MetaFeatureManager() {}
    //��ʼ��
    int init(MetaFeatureDataCenter & data_center, BaseMetaFeatureVector & base_feature_list, std::vector<const char*>& feature_name_list);
    //��ʼ������ݲ�ͬ��Ҫ��ȡ��ͬ�Ĳ���
    int add_action(uint32_t feature_offset, MetaFeatureAction & feature_action, uint32_t flag_cover);
    int get_action(const BaseMetaFeatureVector & base_feature_list, MetaFeatureAction & feature_action, uint32_t flag_cover = METAFEATURE_FLAG_ALL);
    //����OneCase����Դ����
    int generate_meta_features(const OneCase & one_case, const MetaFeatureAction & feature_action, RegressionData& reg_data) const;
    //��ȡ����
    int get_meta_data_info(uint32_t feature_offset, RegressionData& reg_data, MetaFeatureDataInfo & info) const;
    //��ȡbase_feature_list��flag������feature_manager������flag��ȡ
    uint32_t get_feature_flag(BaseMetaFeatureVector base_feature_list) const;

    int get_debug_info(uint32_t feature_offset, void * feature_ptr, char * set_name, char * feature_name, uint32_t limit) const;

#ifdef FEATURE_TIME_DEBUG_
    void get_time_debug(long*& time_list, uint32_t& time_num, uint32_t& calc_times);
#endif
private:
    //��ʼ��Դ�����Ļ�����������������˳��
    int create(void);
    int valid_feature(uint32_t offset, MetaFeatureDataCenter & data_center);
    uint32_t trans_flag(uint32_t offset);
    void add_meta_feature(BaseMetaFeature * new_feature);
    uint32_t ana_base_feature(uint32_t feature_offset);
    int import_onecase_query(const OneCase & onecase, RegressionData& reg_data) const;
    int import_onecase_ad(const OneCase & onecase, RegressionData& reg_data) const;
    int import_onecase_ad_query_match(const OneCase & onecase, RegressionData& reg_data) const;
    int import_onecase_visit_user(const OneCase & onecase, RegressionData& reg_data) const;
    int import_onecase_show(const OneCase & onecase, RegressionData& reg_data) const;
    void take_action(bool is_query_related, const MetaFeatureAction & feature_action, RegressionData& reg_data) const;
    //�˴�����������list�������Ѿ��������򣬲�Uniq���������������
    void get_match_feature(const Uint32List src_list, const Uint32List matching_list,
                           Uint32List& match_dest_list, Uint32List& src_nonmatch_list, Uint32List& matching_nonmatch_list, RegressionData& reg_data) const;
    void get_match_feature(const Uint32List src_list, const Uint32List matching_list1, const Uint32List matching_list2,
                           Uint32List& match_dest_list, Uint32List& src_nonmatch_list, Uint32List& matching_nonmatch_list, RegressionData& reg_data) const;
    void get_uniq(Uint32List src_list, Uint32List& dest_list,  RegressionData& reg_data) const;
    void fill_with_keyword(const Uint32List src_list, const Uint32List insert_list, Uint32List& desc_list, RegressionData& reg_data) const;
    void fill_with_keyword(const char* src, uint32_t src_len, const char* insert, uint32_t insert_len, char*& dest, uint32_t& dest_space, RegressionData& reg_data) const;
    void process_user_info(const OneCase& one_case, RegressionData& reg_data) const;
    void process_user_info_for_session_feature(const OneCase& one_case, const std::vector<UserPvInfo>& pv_list, RegressionData& reg_data, uint32_t const_30m = 1800) const;
    MetaFeatureInfo meta_feature_list_[sizeof(MetaFeatureData)];///�������б�
    uint32_t meta_feature_num_;///����������
    uint32_t now_feature_index_;
    MetaFeatureClassInfo info_list_[sizeof(MetaFeatureData)];///������Ϣ list

    /*
    MetaFeatureData meta;
    uint8_t * pool_;
    uint32_t pool_size_;

    uint8_t * pool_query_ptr_;
    uint8_t * pool_top_;
    */


    bool is_init_;///�Ƿ��Ѿ���ʼ��
    bool meta_add_error_flag_;
    static bool log_flag_;

#ifdef FEATURE_TIME_DEBUG_
    long* meta_feature_time_list_;
    uint32_t calc_times_;
#endif
};
}
#endif
