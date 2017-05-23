#include "MetaFeatureManager.h"
#include "RegressionModel/MetaFeatures/TermMatchMetaFeature.h"
#include "RegressionModel/MetaFeatures/TermMatchSelectedMetaFeature.h"
#include "RegressionModel/MetaFeatures/QueryClusterMetaFeature.h"
#include "RegressionModel/MetaFeatures/QueryConceptMetaFeature.h"
#include "RegressionModel/MetaFeatures/LdaMetaFeature.h"
#include "RegressionModel/MetaFeatures/RegionMetaFeature.h"
#include "RegressionModel/MetaFeatures/StatCtrMetaFeature.h"
#include "RegressionModel/MetaFeatures/UserLastQueryMetaFeature.h"
#include "RegressionModel/MetaFeatures/UserSessionMetaFeature.h"
#include "RegressionModel/MetaFeatures/UserLongCtrMetaFeature.h"
#include "RegressionModel/MetaFeatures/AdConcurrMetaFeature.h"
#include "RegressionModel/MetaFeatures/QueryKwdClassifyMetaFeature.h"
#include "RegressionModel/MetaFeatures/TimePatternMetaFeature.h"
#include "RegressionModel/MetaFeatures/DescStrLenMetaFeature.h"
#include "RegressionModel/MetaFeatures/ExtensionMetaFeature.h"
#include "RegressionModel/MetaFeatures/RankingMetaFeature.h"
#include "RegressionModel/MetaFeatures/QueryAdClusterCrossMetaFeature.h"
#include "RegressionModel/MetaFeatures/LandingPageQualityMetaFeature.h"
#include "RegressionModel/MetaFeatures/LandingPageMatchMetaFeature.h"
#include "RegressionModel/MetaFeatures/LandingPageLdaSimMetaFeature.h"
#include "ADRCommon/CommonTools.h"
#include "MetaFeatureData.h"
#include <map>
using namespace regression;

#ifdef FEATURE_TIME_DEBUG_
pthread_mutex_t time_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

void str_to_str(void * str, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%s", (char*)str);
}
void term_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "T%x", *(uint32_t*)ptr);
}
void bool_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%u", *(bool*)ptr);
}

void idweight_to_str(void * idweight_ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%d,%f", ((IdWeight*)idweight_ptr)->id, ((IdWeight*)idweight_ptr)->weight);
}
void termpair_to_str(void * termpair_ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%x,%x", ((TermPair*)termpair_ptr)->first, ((TermPair*)termpair_ptr)->second);
}

void int_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%d", *(int*)ptr);
}

void uint8_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%d", *(uint8_t*)ptr);
}

void uint64_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%lu", *(uint64_t*)ptr);
}

void user_ad_info_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%u", ((UserAdInfo*)ptr)->adid);
}

void cateinfofixed_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%u,%u,%f", ((CateInfoFixed*)ptr)->cate_term_num, ((CateInfoFixed*)ptr)->all_term_num, ((CateInfoFixed*)ptr)->entr);
}

void cateinfovar_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%u,%f", ((CateInfoVar*)ptr)->cate_index, ((CateInfoVar*)ptr)->value);
}

void float_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%f", *(float*)ptr);
}

void uint64_to_str_hex(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%016lx", *(uint64_t*)ptr);
}

void slice_pair_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%u,%d", ((std::pair<uint32_t, bool>*)ptr)->first, ((std::pair<uint32_t, bool>*)ptr)->second);
}

void term_pos_to_str(void * ptr, char * buf, uint32_t limit)
{
    snprintf(buf, limit, "%u,%u,%u", ((TermDoublePos*)ptr)->termid, ((TermDoublePos*)ptr)->query_pos, ((TermDoublePos*)ptr)->ad_pos);
}

void UserHistoryAdInfo_to_str(void* ptr, char* buf, uint32_t limit)
{
    snprintf(buf, limit, "%u,%u,%u,%u,%u", ((UserHistoryAdInfo*)ptr)->accid,
                                           ((UserHistoryAdInfo*)ptr)->adid,
                                           ((UserHistoryAdInfo*)ptr)->category,
                                           ((UserHistoryAdInfo*)ptr)->click,
                                           ((UserHistoryAdInfo*)ptr)->visit_time_gap);
}

void topicinfo_to_str(void* ptr, char* buf, uint32_t limit)
{
    snprintf(buf, limit, "%u:%f", ((TopicInfo*)ptr)->topic_id, ((TopicInfo*)ptr)->prob);
}

MetaFeatureClassInfo basic_feature_list[] = 
{
    /// Query 
    {"QueryStr", METAOFFSETOF(query.query_str), METAOFFSETOF(query.query_space), sizeof(char), str_to_str},
    {"Query_space", METAOFFSETOF(query.query_space), 0, sizeof(uint32_t), int_to_str},
    {"QueryMd5", METAOFFSETOF(query.query_md5), 0, sizeof(uint64_t), uint64_to_str_hex},
    {"QueryOriginTerms", METAOFFSETOF(query.query_origin_terms.list), METAOFFSETOF(query.query_origin_terms.num), sizeof(uint32_t), term_to_str},
    {"QueryMergedTerms", METAOFFSETOF(query.query_merged_terms.list), METAOFFSETOF(query.query_merged_terms.num), sizeof(uint32_t), term_to_str},
    {"QueryExtension", METAOFFSETOF(query.query_extension.list), METAOFFSETOF(query.query_extension.num), sizeof(IdWeight), idweight_to_str},
    /// Ad
    
    {"KwdStr", METAOFFSETOF(ad.key_str), METAOFFSETOF(ad.key_space), sizeof(char), str_to_str},
    {"KwdLen", METAOFFSETOF(ad.key_space), 0, sizeof(uint32_t), int_to_str},
    {"KwdMd5", METAOFFSETOF(ad.key_md5), 0, sizeof(uint64_t), uint64_to_str_hex},
    {"Accid", METAOFFSETOF(ad.account_id), 0, sizeof(uint32_t), int_to_str},
    {"Kid", METAOFFSETOF(ad.kid), 0, sizeof(uint32_t), int_to_str},
    {"Groupid", METAOFFSETOF(ad.group_id), 0, sizeof(uint32_t), int_to_str},
    {"KeyTitle_space", METAOFFSETOF(ad.keytitle_space), 0, sizeof(uint32_t), int_to_str},
    {"Cid", METAOFFSETOF(ad.creative_id), 0, sizeof(uint32_t), int_to_str},
    {"KeyOriginTerms", METAOFFSETOF(ad.key_origin_terms.list), METAOFFSETOF(ad.key_origin_terms.num), sizeof(uint32_t), term_to_str},
    {"KeyMergedTerms", METAOFFSETOF(ad.key_merged_terms.list), METAOFFSETOF(ad.key_merged_terms.num), sizeof(uint32_t), term_to_str},
    {"TitleOriginTerms", METAOFFSETOF(ad.title_origin_terms.list), METAOFFSETOF(ad.title_origin_terms.num), sizeof(uint32_t), term_to_str},
    {"TitleMergedTerms", METAOFFSETOF(ad.title_merged_terms.list), METAOFFSETOF(ad.title_merged_terms.num), sizeof(uint32_t), term_to_str},
    {"KeyTitleStr", METAOFFSETOF(ad.keytitle_str), METAOFFSETOF(ad.keytitle_space), sizeof(char), str_to_str},
    {"KeyTitldMd5", METAOFFSETOF(ad.keytitle_md5), 0, sizeof(uint64_t), uint64_to_str_hex},
    {"KeyTitleOriginTerms", METAOFFSETOF(ad.keytitle_origin_terms.list), METAOFFSETOF(ad.keytitle_origin_terms.num), sizeof(uint32_t), term_to_str},
    {"KeyTitleMergedTerms", METAOFFSETOF(ad.keytitle_merged_terms.list), METAOFFSETOF(ad.keytitle_merged_terms.num), sizeof(uint32_t), term_to_str},
    {"KeyMatchTitle", METAOFFSETOF(ad.key_match_title.list), METAOFFSETOF(ad.key_match_title.num), sizeof(uint32_t), term_to_str},
    {"KeyNomatchTitle", METAOFFSETOF(ad.key_nomatch_title.list), METAOFFSETOF(ad.key_nomatch_title.num), sizeof(uint32_t), term_to_str},
    {"TitleNomatchKey", METAOFFSETOF(ad.title_nomatch_key.list), METAOFFSETOF(ad.title_nomatch_key.num), sizeof(uint32_t), term_to_str},
    {"AdCategory", METAOFFSETOF(ad.category), 0, sizeof(uint32_t), int_to_str},
    {"AdCategory-L1", METAOFFSETOF(ad.category_L1), 0, sizeof(uint32_t), int_to_str},
    //{"ShowURLDomain", METAOFFSETOF(ad.showurl_domain), METAOFFSETOF(ad.showurl_domain_space), sizeof(char), str_to_str},
    //{"ShowURLDomain_MD5", METAOFFSETOF(ad.showurl_domain_md5), 0, sizeof(uint64_t), uint64_to_str_hex},
    {"KeyExtension", METAOFFSETOF(ad.key_extension.list), METAOFFSETOF(ad.key_extension.num), sizeof(IdWeight), idweight_to_str},

    /// Match
    {"Position", METAOFFSETOF(match.pos), 0, sizeof(uint8_t), uint8_to_str},
    {"MatchType", METAOFFSETOF(match.match_type), 0, sizeof(uint32_t), int_to_str},
    {"QueryMatchKey", METAOFFSETOF(match.query_key_match.list), METAOFFSETOF(match.query_key_match.num), sizeof(uint32_t), term_to_str},
    {"QueryNomatchKey", METAOFFSETOF(match.query_key_nomatch.list), METAOFFSETOF(match.query_key_nomatch.num), sizeof(uint32_t), term_to_str},
    {"KeyNomatchQuery", METAOFFSETOF(match.key_query_nomatch.list), METAOFFSETOF(match.key_query_nomatch.num), sizeof(uint32_t), term_to_str},
    {"QueryMatchKeyTitle", METAOFFSETOF(match.query_keytitle_match.list), METAOFFSETOF(match.query_keytitle_match.num), sizeof(uint32_t), term_to_str},
    {"QueryNomatchKeyTitle", METAOFFSETOF(match.query_keytitle_nomatch.list), METAOFFSETOF(match.query_keytitle_nomatch.num), sizeof(uint32_t), term_to_str},
    {"KeyTitleNoMatchQuery", METAOFFSETOF(match.keytitle_query_nomatch.list), METAOFFSETOF(match.keytitle_query_nomatch.num), sizeof(uint32_t), term_to_str},
    
    /// Match 4 ROI
    {"LoadTime", METAOFFSETOF(ad.load_time), 0, sizeof(float), float_to_str},
    {"MainpageLoadTime", METAOFFSETOF(ad.mainpage_load_time), 0, sizeof(float), float_to_str},
    {"InvalidResourceRatio", METAOFFSETOF(ad.invalid_resource_ratio), 0, sizeof(float), float_to_str},
    {"PageRank", METAOFFSETOF(ad.page_rank), 0, sizeof(float), float_to_str},
    
    {"DescOriginTerms", METAOFFSETOF(ad.desc_origin_terms.list), METAOFFSETOF(ad.desc_origin_terms.num), sizeof(uint32_t), term_to_str},
    {"DescMergedTerms", METAOFFSETOF(ad.desc_merged_terms.list), METAOFFSETOF(ad.desc_merged_terms.num), sizeof(uint32_t), term_to_str},
    {"LPTerms", METAOFFSETOF(ad.lp_terms.list), METAOFFSETOF(ad.lp_terms.num), sizeof(uint32_t), term_to_str},
    {"LPTopics", METAOFFSETOF(ad.lp_topic.list), METAOFFSETOF(ad.lp_topic.num), sizeof(uint32_t), int_to_str},

    {"KeyMatchDesc", METAOFFSETOF(ad.key_match_desc.list), METAOFFSETOF(ad.key_match_desc.num), sizeof(uint32_t), term_to_str},
    {"KeyNomatchDesc", METAOFFSETOF(ad.key_nomatch_desc.list), METAOFFSETOF(ad.key_nomatch_desc.num), sizeof(uint32_t), term_to_str},
    //{"DescNomatchKey", METAOFFSETOF(ad.desc_nomatch_key.list), METAOFFSETOF(ad.desc_nomatch_key.num), sizeof(uint32_t), term_to_str},

    {"TitleMatchLp", METAOFFSETOF(ad.title_match_lp.list), METAOFFSETOF(ad.title_match_lp.num), sizeof(uint32_t), term_to_str},
    {"TitleNomatchLp", METAOFFSETOF(ad.title_nomatch_lp.list), METAOFFSETOF(ad.title_nomatch_lp.num), sizeof(uint32_t), term_to_str},
    //{"LpNomatchTitle", METAOFFSETOF(ad.lp_nomatch_title.list), METAOFFSETOF(ad.lp_nomatch_title.num), sizeof(uint32_t), term_to_str},

    {"DescMatchLp", METAOFFSETOF(ad.desc_match_lp.list), METAOFFSETOF(ad.desc_match_lp.num), sizeof(uint32_t), term_to_str},
    {"DescNomatchLp", METAOFFSETOF(ad.desc_nomatch_lp.list), METAOFFSETOF(ad.desc_nomatch_lp.num), sizeof(uint32_t), term_to_str},
    //{"LpNomatchDesc", METAOFFSETOF(ad.lp_nomatch_desc.list), METAOFFSETOF(ad.lp_nomatch_desc.num), sizeof(uint32_t), term_to_str},

    /// User
    /*
    {"UserID", METAOFFSETOF(user.user_id), METAOFFSETOF(user.user_id_space), sizeof(char), str_to_str},
    {"UserInfo", METAOFFSETOF(user.user_info), 0, sizeof(UserInfo*), uint64_to_str_hex},
    {"User-Session-Pv-Num", METAOFFSETOF(user.session_pv_num), 0, sizeof(uint32_t), int_to_str},
    {"User-Session-AdPv-Num", METAOFFSETOF(user.session_adpv_num), 0, sizeof(uint32_t), int_to_str},
    {"User-Session-AdShow-Num", METAOFFSETOF(user.session_adshow_num), 0, sizeof(uint32_t), int_to_str},
    {"User-Session-AdClick-Num", METAOFFSETOF(user.session_adclick_num), 0, sizeof(uint32_t), int_to_str},
    {"User-Session-QueryTerms", METAOFFSETOF(user.session_query_terms.list),
                                METAOFFSETOF(user.session_query_terms.num),
                                sizeof(uint32_t),
                                int_to_str},
    {"User-Session-QueryMergedTerms", METAOFFSETOF(user.session_query_merged_terms.list),
                                      METAOFFSETOF(user.session_query_merged_terms.num),
                                      sizeof(uint32_t),
                                      int_to_str},
    {"User-Session-Ads", METAOFFSETOF(user.session_ads.list),
                         METAOFFSETOF(user.session_ads.num),
                         sizeof(UserHistoryAdInfo),
                         UserHistoryAdInfo_to_str},
    {"UserLastQueryStr", METAOFFSETOF(user.last_query_str), METAOFFSETOF(user.last_query_space), sizeof(char), str_to_str},
    {"UserLastQuerySpace", METAOFFSETOF(user.last_query_space), 0, sizeof(uint32_t), int_to_str},
    {"User-LastQueryTerms", METAOFFSETOF(user.last_query_terms.list),
                            METAOFFSETOF(user.last_query_terms.num),
                            sizeof(uint32_t),
                            int_to_str},
    {"User-LastQueryMergedTerms", METAOFFSETOF(user.last_query_merged_terms.list),
                                  METAOFFSETOF(user.last_query_merged_terms.num),
                                  sizeof(uint32_t), term_to_str},
    {"User-LastQueryAds", METAOFFSETOF(user.last_query_ads.list), METAOFFSETOF(user.last_query_ads.num),
                          sizeof(UserHistoryAdInfo), UserHistoryAdInfo_to_str},
    {"User-LastQuery-CurrentQuery-Match", METAOFFSETOF(user.lastQuery_currentQuery_match.list),
                                          METAOFFSETOF(user.lastQuery_currentQuery_match.num),
                                          sizeof(uint32_t), term_to_str},
    {"User-LastQuery-CurrentQuery-NoMatch", METAOFFSETOF(user.lastQuery_currentQuery_nomatch.list),
                                            METAOFFSETOF(user.lastQuery_currentQuery_nomatch.num),
                                            sizeof(uint32_t), term_to_str},
    {"User-CurrentQuery-LastQuery-Nomatch", METAOFFSETOF(user.currentQuery_lastQuery_nomatch.list),
                                            METAOFFSETOF(user.currentQuery_lastQuery_nomatch.num),
                                            sizeof(uint32_t), term_to_str},
    {"User-LastQuery-AdShowNum", METAOFFSETOF(user.last_query_adshow_num), 0, sizeof(uint32_t), int_to_str},
    {"User-LastQuery-AdClickNum", METAOFFSETOF(user.last_query_adclick_num), 0, sizeof(uint32_t), int_to_str},
    {"User-SessionQuery-CurrentQuery-Match", METAOFFSETOF(user.sessionQuery_currentQuery_match.list),
                                             METAOFFSETOF(user.sessionQuery_currentQuery_match.num),
                                             sizeof(uint32_t), term_to_str},
    {"User-SessionQuery-CurrentQuery-Nomatch", METAOFFSETOF(user.sessionQuery_currentQuery_nomatch.list),
                                               METAOFFSETOF(user.sessionQuery_currentQuery_nomatch.num),
                                               sizeof(uint32_t), term_to_str},
    {"User-CurrentQuery-SessionQuery-Nomatch", METAOFFSETOF(user.currentQuery_sessionQuery_nomatch.list),
                                               METAOFFSETOF(user.currentQuery_sessionQuery_nomatch.num),
                                               sizeof(uint32_t), term_to_str},
    {"User-Pv-Gap-From-Last-Click", METAOFFSETOF(user.pv_gap_from_last_click), 0, sizeof(uint32_t), int_to_str},
    {"User-Is-Cate3-Clicked", METAOFFSETOF(user.is_cate3_clicked), 0, sizeof(bool), int_to_str},
    {"User-Last3Pv-Click-Cate3-List", METAOFFSETOF(user.last3pv_click_cate3_list.list),
                                      METAOFFSETOF(user.last3pv_click_cate3_list.num),
                                      sizeof(uint32_t),
                                      int_to_str},
    {"User-Long-PvNum", METAOFFSETOF(user.user_long_pv_num), 0, sizeof(float), float_to_str},
    {"User-Long-CkNum", METAOFFSETOF(user.user_long_ck_num), 0, sizeof(float), float_to_str},
    {"User-Long-Cate-Pv", METAOFFSETOF(user.user_long_cate_pv), 0, sizeof(uint32_t), int_to_str},
    {"User-Long-L1-Cate", METAOFFSETOF(user.user_long_l1_cate.list), METAOFFSETOF(user.user_long_l1_cate.num), sizeof(uint32_t), int_to_str},
    {"User-Long-Best-L1-Cate", METAOFFSETOF(user.user_long_best_l1_cate), 0, sizeof(uint32_t), int_to_str},
    {"User-Long-Worst-L1-Cate", METAOFFSETOF(user.user_long_worst_l1_cate), 0, sizeof(uint32_t), int_to_str},
    {"User-Long-Lda", METAOFFSETOF(user.user_long_lda.list), METAOFFSETOF(user.user_long_lda.num), sizeof(uint32_t), int_to_str},*/
    /// Visit
    /*
    {"IP", METAOFFSETOF(visit.ip), 0, sizeof(uint32_t), int_to_str},
    {"PID", METAOFFSETOF(visit.pid), METAOFFSETOF(visit.pid_space), sizeof(char), str_to_str},
    {"PIDLen", METAOFFSETOF(visit.pid_space), 0, sizeof(uint32_t), int_to_str},
    {"ServiceType", METAOFFSETOF(visit.service_type), 0, sizeof(uint32_t), int_to_str},
    {"RegionProvince", METAOFFSETOF(visit.region_province), 0, sizeof(uint32_t), int_to_str},
    {"Hour", METAOFFSETOF(visit.hour), 0, sizeof(uint32_t), int_to_str},
    {"AbsoluteTime", METAOFFSETOF(visit.time), 0, sizeof(uint32_t), int_to_str},*/
    /// Show
    /*
    {"IsLeft", METAOFFSETOF(show.is_left), 0, sizeof(uint8_t), uint8_to_str},
    {"IsBlue", METAOFFSETOF(show.is_blue), 0, sizeof(uint8_t), uint8_to_str},
    {"IsCrown", METAOFFSETOF(show.is_crown), 0, sizeof(uint8_t), uint8_to_str},
    {"NoCrown", METAOFFSETOF(show.no_crown), 0, sizeof(uint8_t), uint8_to_str} */
    {"Mousetype", METAOFFSETOF(ad.mouse_type_list.list), METAOFFSETOF(ad.mouse_type_list.num), sizeof(uint32_t), int_to_str},
    {"Mousenum", METAOFFSETOF(ad.mouse_num_list.list), METAOFFSETOF(ad.mouse_num_list.num), sizeof(uint32_t), int_to_str},
    {"Mousetime", METAOFFSETOF(ad.mouse_time_list.list), METAOFFSETOF(ad.mouse_time_list.num), sizeof(uint32_t), int_to_str},
    {"Mousedis", METAOFFSETOF(ad.mouse_distance_list.list), METAOFFSETOF(ad.mouse_distance_list.num), sizeof(uint32_t), int_to_str}
};

bool MetaFeatureManager::log_flag_ = false;

int MetaFeatureManager::get_debug_info(uint32_t feature_offset, void * feature_ptr, char * set_name, char * feature_name, uint32_t limit) const
{
    const MetaFeatureClassInfo & info = info_list_[feature_offset];
    if (info.feature_name == NULL || info.tostr_fun == NULL)
    { return -1; }
    snprintf(set_name, limit, "%s", info.feature_name);
    info.tostr_fun(feature_ptr, feature_name, limit);

    return 0;
}

bool less_meta_feature(const MetaFeatureInfo& a, const MetaFeatureInfo& b)
{
    if (a.max_origin_base_feature != b.max_origin_base_feature)
    { return a.max_origin_base_feature < b.max_origin_base_feature; }
    return a.meta_feature_index < b.meta_feature_index;
}
int MetaFeatureManager::create(void)
{
    meta_feature_num_ = 0;
    //初始化info_list
    memset(info_list_, 0, sizeof(info_list_));
    //注册特征
    /// Term
    //add_meta_feature(new QueryKeyTitleSynonymPairAllMetaFeature());
    //add_meta_feature(new QueryKeyTitleSynonymPairNomatchMetaFeature());
    //add_meta_feature(new QuerySynonymHitMetaFeature());
    //add_meta_feature(new QuerySynonymNotHitMetaFeature());
    //add_meta_feature(new KeyTitleSynonymHitMetaFeature());
    //add_meta_feature(new KeyTitleSynonymNotHitMetaFeature());
    /// Selected Term
    
    add_meta_feature(new QuerySelectedTermsMetaFeature());
    add_meta_feature(new KeyTitleSelectedTermsMetaFeature());
    add_meta_feature(new KeySelectedTermsMetaFeature());
    add_meta_feature(new TitleSelectedTermsMetaFeature());
    add_meta_feature(new QueryKeyTitleMatchSelectedMetaFeature());
    add_meta_feature(new QueryKeyTitleNomatchSelectedMetaFeature());
    add_meta_feature(new KeyTitleQueryNomatchSelectedMetaFeature());
    add_meta_feature(new QueryKeyTitleSynonymPairSelectedMetaFeature());
    add_meta_feature(new QuerySynonymHitTermSelectedMetaFeature());
    add_meta_feature(new QuerySynonymNotHitTermSelectedMetaFeature());
    add_meta_feature(new KeyTitleSynonymHitTermSelectedMetaFeature());
    add_meta_feature(new KeyTitleSynonymNotHitTermSelectedMetaFeature());
    
    /// QueryCluster
    add_meta_feature(new QueryClusterMetaFeature());
    /// QueryConcept
    add_meta_feature(new QueryConceptMetaFeature());
    /// LDA
    add_meta_feature(new QueryLdaDistrMetaFeature());
    add_meta_feature(new AdLdaDistrMetaFeature());
    add_meta_feature(new QueryLdaTopicMetaFeature());
    add_meta_feature(new AdLdaTopicMetaFeature());
    add_meta_feature(new QueryMatchAdLdaTopicMetaFeature());
    add_meta_feature(new QueryNomatchAdLdaTopicMetaFeature());
    add_meta_feature(new AdNomatchQueryLdaTopicMetaFeature());
    add_meta_feature(new QueryAdLdaCosineSimMetaFeature());



    // ROI features
    add_meta_feature(new LandingPageQualityMetaFeature());        // landingpage quality
    add_meta_feature(new KeyTitleSynonymMatchMetaFeature());      // Key-Title Synonym Match
    
    add_meta_feature(new KeyDescSynonymMatchMetaFeature());      // Key-Desc Synonym Match
    add_meta_feature(new KeyLpSynonymMatchMetaFeature());         // Key-Lp Synonym Match
    add_meta_feature(new TitleDescLpSynonymMatchMetaFeature());   // TitleDest-Lp Synonym Match

    add_meta_feature(new TitleLdaDistrMetaFeature()); // these four features are used to present ad text
    add_meta_feature(new TitleLdaTopicMetaFeature());
    add_meta_feature(new DescLdaDistrMetaFeature());
    add_meta_feature(new DescLdaTopicMetaFeature());
    
    add_meta_feature(new KeyMatchTitleLdaTopicMetaFeature()); 
    add_meta_feature(new KeyNomatchTitleLdaTopicMetaFeature());
    add_meta_feature(new KeyMatchDescLdaTopicMetaFeature());
    add_meta_feature(new KeyNomatchDescLdaTopicMetaFeature());
    
    add_meta_feature(new DescMatchLpLdaTopicMetaFeature());
    add_meta_feature(new DescNomatchLpLdaTopicMetaFeature());
    add_meta_feature(new TitleMatchLpLdaTopicMetaFeature());
    add_meta_feature(new TitleNomatchLpLdaTopicMetaFeature());

    add_meta_feature(new AdLdaCosineSimMetaFeature());

    /*
    /// Region
    add_meta_feature(new QueryContentRegionMetaFeature());
    add_meta_feature(new AdContentRegionMetaFeature());
    add_meta_feature(new VisitRegionMetaFeature());
    
    /// Concurr
    add_meta_feature(new QueryConcurrTopicMetaFeature());
    add_meta_feature(new KWDConcurrTopicMetaFeature());
    add_meta_feature(new AdConcurrCateMetaFeature()); */
    add_meta_feature(new QueryClassifyMetaFeature());
    add_meta_feature(new KwdClassifyMetaFeature());
    /*
    add_meta_feature(new TimePatternMetaFeature());
    add_meta_feature(new DescStrLenMetaFeature());
    /// StatCtr
    add_meta_feature(new QueryCtrMetaFeature());
    add_meta_feature(new QueryAccidCtrMetaFeature());
    add_meta_feature(new PidCtrMetaFeature());
    add_meta_feature(new AccidRegionCtrMetaFeature());
    /// User
    add_meta_feature(new UserLastQueryTransformTypeMetaFeature());
    add_meta_feature(new UserLastQuerySelectedTermsMetaFeature());
    add_meta_feature(new UserLastQueryLdaDistrMetaFeature());
    add_meta_feature(new UserLastQueryLdaTopicMetaFeature());
    add_meta_feature(new UserLastQueryLdaCosineSimMetaFeature());
    add_meta_feature(new UserLastQueryLdaCommonMetaFeature());
    add_meta_feature(new UserLastQueryFlagMetaFeature());
    add_meta_feature(new UserLastQueryCurrentQueryCosineSimMetaFeature());
    add_meta_feature(new UserSessionSelectedTermsMetaFeature());
    add_meta_feature(new UserSessionLdaDistrMetaFeature());
    add_meta_feature(new UserSessionLdaTopicMetaFeature());
    add_meta_feature(new UserSessionLdaCosineSimMetaFeature());
    add_meta_feature(new UserSessionCommonLdaTopicMetaFeature());
    add_meta_feature(new UserSessionClickCategoryMetaFeature());
    add_meta_feature(new UserSessionClickAccidMetaFeature());
    add_meta_feature(new UserSessionClickKidMetaFeature());
    add_meta_feature(new UserSessionQueryCurrentQueryCosineSimMetaFeature());
    add_meta_feature(new UserLongCtrMetaFeature());
    */
    // extension
    add_meta_feature(new QueryExtensionSelectedMetaFeature());
    add_meta_feature(new KeyExtensionSelectedMetaFeature());
    add_meta_feature(new QueryMatchKeyExtensionMetaFeature());
    add_meta_feature(new QueryNomatchKeyExtensionMetaFeature());
    add_meta_feature(new KeyNomatchQueryExtensionMetaFeature());
    add_meta_feature(new QueryKeyExtensionCosineSimMetaFeature());

    add_meta_feature(new QueryKeyMatchWholeMetaFeature());
    add_meta_feature(new QueryKeyTitleMatchWholeMetaFeature());

    add_meta_feature(new QueryAdClusterCrossMetaFeature());
    //add_meta_feature(new QueryAdMatchRankMetaFeature());


    for (uint32_t i = 0; i < sizeof(basic_feature_list) / sizeof(MetaFeatureClassInfo); i++)
    {
        if (info_list_[basic_feature_list[i].offset].feature_name != NULL)
        {
            ADR_ERROR("MetaFeature Offset Conflits: %s\t%s\t%u\n", basic_feature_list[i].feature_name, info_list_[basic_feature_list[i].offset].feature_name, basic_feature_list[i].offset);
            meta_add_error_flag_ = true;
        }
        info_list_[basic_feature_list[i].offset] = basic_feature_list[i];
        info_list_[basic_feature_list[i].offset].flag = trans_flag(basic_feature_list[i].offset);
        info_list_[basic_feature_list[i].offset].feature_index = FEATURE_INDEX_ORIGIN;
        if (basic_feature_list[i].tostr_fun == NULL)
        {
            ADR_ERROR("Tostr function Not specified:\t%d\t%s\n", basic_feature_list[i].offset, basic_feature_list[i].feature_name);
            return -1;
        }
        if (!log_flag_)
        {
            ADR_INFO("[BASIC OFFSET INFO]\t%d\t%s\n", basic_feature_list[i].offset, basic_feature_list[i].feature_name);
        }
    }
    if (meta_add_error_flag_)
    {
        return -1;
    }
    //计算依赖特征
    for (uint32_t i = 0; i < meta_feature_num_; i++)
    {
        if (!log_flag_)
        {
            ADR_INFO("[EXPAND OFFSET INFO]\t%d\t%s\n", meta_feature_list_[i].feature_offset, info_list_[meta_feature_list_[i].feature_offset].feature_name);
        }
        if (meta_feature_list_[i].max_origin_base_feature == MAX_ORIGIN_BASE_FEATURE_ORIGIN)
        {
            if (ana_base_feature(meta_feature_list_[i].feature_offset) == MAX_ORIGIN_BASE_FEATURE_ERROR)
            {
                ADR_ERROR("MetaFeature Varify ERROR! offset: %u\t%s\n", meta_feature_list_[i].feature_offset, info_list_[meta_feature_list_[i].feature_offset].feature_name);
                return -1;
            }
        }
    }
    std::sort(meta_feature_list_ , meta_feature_list_ + meta_feature_num_, less_meta_feature);
#ifdef FEATURE_DEBUG_
    if (!log_flag_)
    {
        for (uint32_t i = 0; i < meta_feature_num_; i++)
        {
            ADR_INFO("[TOPOLOGY]\t%s\t%u\t%u\n", info_list_[meta_feature_list_[i].feature_offset].feature_name, meta_feature_list_[i].max_origin_base_feature, meta_feature_list_[i].meta_feature_index);
        }
    }
#endif
    for (uint32_t i = 0; i < meta_feature_num_; i++)
    {
        meta_feature_list_[i].meta_feature_index = i;
        info_list_[meta_feature_list_[i].feature_offset].feature_index = i;
    }
    log_flag_ = true;
#ifdef FEATURE_TIME_DEBUG_
    meta_feature_time_list_ = (long*)calloc(sizeof(MetaFeatureData), sizeof(long));
    calc_times_ = 0;
#endif
    return 0;
}

void MetaFeatureManager::add_meta_feature(BaseMetaFeature * new_feature)
{
    MetaFeatureInfo * tmp_meta_feature = meta_feature_list_ + meta_feature_num_;///临时的特征类
    uint32_t feature_offset = new_feature->get_feature_info()->offset;///特征offset，作为特征ID
    if (info_list_[feature_offset].feature_name != NULL)
    {
        ADR_ERROR("MetaFeature Offset Conflits: %s\t%s\t%u\n", new_feature->get_feature_info()->feature_name, info_list_[feature_offset].feature_name, feature_offset);
        meta_add_error_flag_ = true;
    }
    info_list_[feature_offset] = *new_feature->get_feature_info();
    info_list_[feature_offset].feature_index = meta_feature_num_;
    info_list_[feature_offset].flag = 0;
    tmp_meta_feature->feature_ptr = new_feature;//获取特征类指针
    tmp_meta_feature->feature_offset = feature_offset;//获取特征ID
    tmp_meta_feature->meta_feature_index = FEATURE_INDEX_ORIGIN;///初始化
    tmp_meta_feature->max_origin_base_feature = MAX_ORIGIN_BASE_FEATURE_ORIGIN;///初始化
    meta_feature_num_++;
}

uint32_t MetaFeatureManager::ana_base_feature(uint32_t feature_offset)
{

    //获取对象
    MetaFeatureClassInfo & info = info_list_[feature_offset];
    //检查是否为特征
    if (info.feature_name == NULL)
    {
        ADR_ERROR("MetaFeature Not Defined: %u!\n", feature_offset);
        return MAX_ORIGIN_BASE_FEATURE_ERROR;
    }
    
    //检查是否基础特征，基础特征直接返回offset
    if (info.feature_index == FEATURE_INDEX_ORIGIN)
    {
        info.flag = trans_flag(feature_offset);
        return feature_offset;
    }
    //非基础特征，若已经计算过返回计算结果
    MetaFeatureInfo & meta_feature = meta_feature_list_[info.feature_index];
    if (meta_feature.max_origin_base_feature != MAX_ORIGIN_BASE_FEATURE_ORIGIN)
    { return meta_feature.max_origin_base_feature; }
    //为避免出现循环依赖，先置为错误
    meta_feature.max_origin_base_feature = MAX_ORIGIN_BASE_FEATURE_ERROR;
    uint32_t base_feature_offset = 0;
    for (uint32_t i = 0; i < info.base_feature_num; i++)
    {
        uint32_t tmp_base_feature = ana_base_feature(info.base_feature_list[i]);
        info.flag |= info_list_[info.base_feature_list[i]].flag;
        if (tmp_base_feature > base_feature_offset)
        {
            base_feature_offset = tmp_base_feature;
        }
    }
    //检查特征的位置是否与其依赖的基础特征匹配
    uint32_t check_flag = trans_flag(feature_offset);
    if ((info.flag & METAFEATURE_FLAG_SHOW) == METAFEATURE_FLAG_SHOW)
    {
        if (check_flag != METAFEATURE_FLAG_SHOW)
        {
            ADR_ERROR("METAFEATURE_FLAG_SHOW feature offset error! %u\t%s\t%u\n", feature_offset, info.feature_name, check_flag);
            return MAX_ORIGIN_BASE_FEATURE_ERROR;
        }
    }
    if ((info.flag & METAFEATURE_FLAG_VISIT_USER) == METAFEATURE_FLAG_VISIT_USER)
    {
        if (check_flag != METAFEATURE_FLAG_VISIT_USER)
        {
            ADR_ERROR("METAFEATURE_FLAG_VISIT_USER feature offset error! %u\t%s\t%u\n", feature_offset, info.feature_name, check_flag);
            return MAX_ORIGIN_BASE_FEATURE_ERROR;
        }
    }
    else if ((info.flag & METAFEATURE_FLAG_AD_MATCH) == METAFEATURE_FLAG_AD_MATCH)
    {
        if (check_flag != METAFEATURE_FLAG_AD_MATCH)
        {
            ADR_ERROR("METAFEATURE_FLAG_AD_MATCH feature offset error! %u\t%s\t%u\n", feature_offset, info.feature_name, check_flag);
            return MAX_ORIGIN_BASE_FEATURE_ERROR;
        }
    }
    else if ((info.flag & METAFEATURE_FLAG_AD) == METAFEATURE_FLAG_AD)
    {
        if (check_flag != METAFEATURE_FLAG_AD)
        {
            ADR_ERROR("METAFEATURE_FLAG_AD feature offset error! %u\t%s\t%u\n", feature_offset, info.feature_name, check_flag);
            return MAX_ORIGIN_BASE_FEATURE_ERROR;
        }
    }
    else if ((info.flag & METAFEATURE_FLAG_QUERY) == METAFEATURE_FLAG_QUERY)
    {
        if (check_flag != METAFEATURE_FLAG_QUERY)
        {
            ADR_ERROR("METAFEATURE_FLAG_QUERY feature offset error! %u\t%s\t%u\n", feature_offset, info.feature_name, check_flag);
            return MAX_ORIGIN_BASE_FEATURE_ERROR;
        }
    }
    //若无基础特征，错误
    if (base_feature_offset == 0)
    { return MAX_ORIGIN_BASE_FEATURE_ERROR; }
    //设置特征依赖最大基础特征offset，相同基础特征offset，拓扑排序
    meta_feature.meta_feature_index = now_feature_index_ ++;
    meta_feature.max_origin_base_feature = base_feature_offset;
    return base_feature_offset;
}

uint32_t MetaFeatureManager::trans_flag(uint32_t offset)
{
    if ((uint32_t)(offset - METAOFFSETOF(show)) < sizeof(ShowMetaFeatureInfo))
    { return METAFEATURE_FLAG_SHOW; }
    if ((uint32_t)(offset - METAOFFSETOF(query)) < sizeof(QueryMetaFeatureInfo))
    { return METAFEATURE_FLAG_QUERY; }
    if ((uint32_t)(offset - METAOFFSETOF(ad)) < sizeof(AdMetaFeatureInfo))
    { return METAFEATURE_FLAG_AD_MATCH; }
    if ((uint32_t)(offset - METAOFFSETOF(match)) < sizeof(MatchMetaFeatureInfo))
    { return METAFEATURE_FLAG_AD_MATCH | METAFEATURE_FLAG_QUERY; }
    if ((uint32_t)(offset - METAOFFSETOF(visit)) < sizeof(VisitMetaFeatureInfo))
    { return METAFEATURE_FLAG_VISIT_USER; }
    if ((uint32_t)(offset - METAOFFSETOF(user)) < sizeof(UserMetaFeatureInfo))
    { return METAFEATURE_FLAG_VISIT_USER; }
    return 0;
}

int MetaFeatureManager::valid_feature(uint32_t offset, MetaFeatureDataCenter & data_center)
{
    MetaFeatureClassInfo & info = info_list_[offset];
    if (info.feature_name == NULL)
    {
        ADR_ERROR("Valid MetaFeature ERROR: %d!\n", offset);
        return -1;
    }
    //非基础特征需要初始化，基础特征只保留flag
    if (info.feature_index != FEATURE_INDEX_ORIGIN)
    {
        //非基础特征将依赖的特征都初始化
        if (meta_feature_list_[info.feature_index].feature_ptr->init(data_center) != 0)
        {
            ADR_ERROR("Init DataCenter Error, MetaIndex: %u, Feature %u\n", offset, info.feature_index);
            return -1;
        }
        for ( uint32_t i = 0; i < info.base_feature_num; i++)
        {
            if (valid_feature(info.base_feature_list[i], data_center) != 0)
            { return -1; }
        }
    }
    info.is_init = true;
    return 0;
}

int MetaFeatureManager::init(MetaFeatureDataCenter & data_center, BaseMetaFeatureVector & base_feature_list, std::vector<const char*>& feature_name_list)
{
    uint32_t i;
    //将依赖特征初始化
    if (create() != 0)
    {
        return -1;
    }
    for ( i = 0; i < base_feature_list.size(); i++)
    {
        if (valid_feature(base_feature_list[i], data_center) != 0)
        {
            ADR_ERROR("Init MetaFeature Error \"%d\" in Feature \"%s\"!\n", base_feature_list[i], feature_name_list[i]);
            return -1;
        }
    }
    is_init_ = true;
    return 0;
}

int MetaFeatureManager::add_action(uint32_t feature_offset, MetaFeatureAction & feature_action, uint32_t flag_cover)
{
    MetaFeatureClassInfo & info = info_list_[feature_offset];
    if (!info.is_init)
    {
        return -1;
    }
    if (info.feature_index != FEATURE_INDEX_ORIGIN)
    {
        if ((info.flag & flag_cover) == info.flag)
        {
            feature_action.feature_list.push_back(meta_feature_list_[info.feature_index]);
        }
        for (uint32_t i = 0; i < info.base_feature_num; i++)
        {
            if (add_action(info.base_feature_list[i], feature_action, flag_cover) != 0)
            { return -1; }
        }
    }
    else
    {
        if ((info.flag & flag_cover) == info.flag)
        {
            feature_action.action_flag |= info.flag;
        }
    }
    return 0;
}

int MetaFeatureManager::get_action(const BaseMetaFeatureVector & base_feature_list, MetaFeatureAction & feature_action, uint32_t flag_cover)
{
    if (!is_init_)
    {
        ADR_ERROR("get_feature_action and no init !\n");
        return -1;
    }
    feature_action.action_flag = 0;
    for (uint32_t i = 0; i < base_feature_list.size(); i++)
    {
        if (add_action(base_feature_list[i], feature_action, flag_cover) != 0)
        { return -1; }
    }
    if (feature_action.feature_list.size() == 0)
    { return 0; }
    std::sort(&feature_action.feature_list[0], &feature_action.feature_list[0] + feature_action.feature_list.size(), less_meta_feature);
    uint32_t index = 0;
    for (uint32_t i = 1; i < feature_action.feature_list.size(); i++)
    {
        if (feature_action.feature_list[i].meta_feature_index != feature_action.feature_list[index].meta_feature_index)
        { feature_action.feature_list[++index] = feature_action.feature_list[i]; }
    }
    feature_action.feature_list.resize( index + 1);
    return 0;
}
uint32_t MetaFeatureManager::get_feature_flag(BaseMetaFeatureVector base_feature_list) const
{
    if (!is_init_)
    {
        ADR_ERROR("get_feature_flag and no init !\n");
        return -1;
    }
    uint32_t i, flag = 0;
    for ( i = 0; i < base_feature_list.size(); i++)
    {
        if (!info_list_[base_feature_list[i]].is_init)
        {
            ADR_ERROR("get_feature_flag feature no init !%d\n", base_feature_list[i]);
            return -1;
        }
        flag |= info_list_[base_feature_list[i]].flag;
    }
    return flag;
}

#define META_ONECASE_CP_ONE(dst,src) {\
    reg_data.meta_data_.dst = src;\
    /*    valid_list[METAOFFSETOF(dst)] = 1;*/\
}

#define META_ONECASE_CP_LIST(dst,dstnum,src,srcnum,type) {\
    reg_data.meta_data_.dst = (type*)reg_data.pool_top_;\
    reg_data.meta_data_.dstnum = srcnum;\
    reg_data.pool_top_ += reg_data.meta_data_.dstnum * sizeof(type);\
    memcpy(reg_data.meta_data_.dst,src,reg_data.meta_data_.dstnum * sizeof(type));\
    /*    valid_list[METAOFFSETOF(dst)] = 1;*/\
}

#define META_ONECASE_CP_LIST_D(dst,dstnum,src,srcnum,type) {\
    reg_data.meta_data_.dst = (type*)src;\
    reg_data.meta_data_.dstnum = srcnum;\
    /*    valid_list[METAOFFSETOF(dst)] = 1;*/\
}

int MetaFeatureManager::import_onecase_query(const OneCase & one_case, RegressionData& reg_data) const
{
    META_ONECASE_CP_LIST_D(query.query_str,
                           query.query_space,
                           one_case.query.query,
                           strlen(one_case.query.query) + 1,
                           char);
    META_ONECASE_CP_ONE(query.query_md5, one_case.query.query_md5);
    META_ONECASE_CP_LIST_D(query.query_origin_terms.list,
                           query.query_origin_terms.num,
                           one_case.query.origin_terms.list,
                           one_case.query.origin_terms.num,
                           uint32_t);
    get_uniq(reg_data.meta_data_.query.query_origin_terms,
             reg_data.meta_data_.query.query_merged_terms,
             reg_data);

    META_ONECASE_CP_LIST_D(query.query_extension.list,
                           query.query_extension.num,
                           one_case.query.query_ext_terms.list,
                           one_case.query.query_ext_terms.num,
                           IdWeight);

#ifdef FEATURE_DEBUG_
    char buffer[65536] = {0};
    uint32_t offset = snprintf(buffer, sizeof(buffer), "%016lx\t%u\tQueryMergedTerms",
                               reg_data.meta_data_.pv.pvid, 0);
    for (uint32_t i = 0; i < reg_data.meta_data_.query.query_merged_terms.num; i++)
    {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\t%u",
                           reg_data.meta_data_.query.query_merged_terms.list[i]);
    }
    ADR_INFO("FD\t%s\n", buffer);
#endif

    return 0;
}

int MetaFeatureManager::import_onecase_ad(const OneCase& one_case, RegressionData& reg_data) const
{
    META_ONECASE_CP_ONE(ad.account_id, one_case.ad.acid);
    META_ONECASE_CP_ONE(ad.group_id, one_case.ad.grpid);
    META_ONECASE_CP_ONE(ad.kid, one_case.ad.adid);
    META_ONECASE_CP_ONE(ad.creative_id, one_case.ad.crid);

    META_ONECASE_CP_LIST_D(ad.key_str,
                           ad.key_space,
                           one_case.ad.key,
                           strlen(one_case.ad.key) + 1,
                           char);
    META_ONECASE_CP_ONE(ad.key_md5, one_case.ad.key_md5);
    META_ONECASE_CP_LIST_D(ad.key_origin_terms.list,
                           ad.key_origin_terms.num,
                           one_case.ad.key_terms.list,
                           one_case.ad.key_terms.num,
                           uint32_t);
    get_uniq(reg_data.meta_data_.ad.key_origin_terms,
             reg_data.meta_data_.ad.key_merged_terms,
             reg_data);

    META_ONECASE_CP_LIST_D(ad.title_str,
                           ad.title_space,
                           one_case.ad.title,
                           strlen(one_case.ad.title) + 1,
                           char);
    META_ONECASE_CP_LIST_D(ad.title_origin_terms.list,
                           ad.title_origin_terms.num,
                           one_case.ad.title_terms.list,
                           one_case.ad.title_terms.num,
                           uint32_t);
    get_uniq(reg_data.meta_data_.ad.title_origin_terms,
             reg_data.meta_data_.ad.title_merged_terms,
             reg_data);

    get_match_feature(reg_data.meta_data_.ad.key_merged_terms,
                      reg_data.meta_data_.ad.title_merged_terms,
                      reg_data.meta_data_.ad.key_match_title,
                      reg_data.meta_data_.ad.key_nomatch_title,
                      reg_data.meta_data_.ad.title_nomatch_key,
                      reg_data);

    fill_with_keyword(reg_data.meta_data_.ad.title_str,
                      reg_data.meta_data_.ad.title_space - 1,
                      reg_data.meta_data_.ad.key_str,
                      reg_data.meta_data_.ad.key_space - 1,
                      reg_data.meta_data_.ad.keytitle_str,
                      reg_data.meta_data_.ad.keytitle_space,
                      reg_data);
    fill_with_keyword(reg_data.meta_data_.ad.title_origin_terms,
                      reg_data.meta_data_.ad.key_origin_terms,
                      reg_data.meta_data_.ad.keytitle_origin_terms,
                      reg_data);
    get_uniq(reg_data.meta_data_.ad.keytitle_origin_terms,
             reg_data.meta_data_.ad.keytitle_merged_terms,
             reg_data);

    reg_data.meta_data_.ad.keytitle_md5 = get_md5_64(reg_data.meta_data_.ad.keytitle_str);

    META_ONECASE_CP_ONE(ad.category, one_case.ad.category);
    //META_ONECASE_CP_LIST_D(ad.showurl_domain,
    //                       ad.showurl_domain_space,
    //                       one_case.ad.show_url,
    //                       strlen(one_case.ad.show_url) + 1,
    //                       char);
    //reg_data.meta_data_.ad.showurl_domain_md5 = get_md5_64(reg_data.meta_data_.ad.showurl_domain);
    reg_data.meta_data_.ad.category_L1 = get_cat_high(reg_data.meta_data_.ad.category);

    META_ONECASE_CP_LIST_D(ad.key_extension.list,
                           ad.key_extension.num,
                           one_case.ad.keyword_ext_terms.list,
                           one_case.ad.keyword_ext_terms.num,
                           IdWeight);

    // roi preorediction info
    META_ONECASE_CP_ONE(ad.load_time, one_case.ad.load_time);
    META_ONECASE_CP_ONE(ad.mainpage_load_time, one_case.ad.mainpage_load_time);
    META_ONECASE_CP_ONE(ad.invalid_resource_ratio, one_case.ad.invalid_resource_ratio);
    META_ONECASE_CP_ONE(ad.invalid_pic_ratio, one_case.ad.invalid_pic_ratio);
    META_ONECASE_CP_ONE(ad.page_rank, one_case.ad.page_rank);

    META_ONECASE_CP_LIST_D(ad.mouse_type_list.list,
                ad.mouse_type_list.num,
                one_case.ad.mouse_type_list.list,
                one_case.ad.mouse_type_list.num,
                uint32_t);
    META_ONECASE_CP_LIST_D(ad.mouse_num_list.list,
                ad.mouse_num_list.num,
                one_case.ad.mouse_num_list.list,
                one_case.ad.mouse_num_list.num,
                uint32_t);
    META_ONECASE_CP_LIST_D(ad.mouse_time_list.list,
                ad.mouse_time_list.num,
                one_case.ad.mouse_time_list.list,
                one_case.ad.mouse_time_list.num,
                uint32_t);
    META_ONECASE_CP_LIST_D(ad.mouse_distance_list.list,
                ad.mouse_distance_list.num,
                one_case.ad.mouse_distance_list.list,
                one_case.ad.mouse_distance_list.num,
                uint32_t);


    META_ONECASE_CP_LIST_D(ad.desc_str,
                           ad.desc_space,
                           one_case.ad.desc,
                           strlen(one_case.ad.desc) + 1,
                           char);
    META_ONECASE_CP_LIST_D(ad.desc_origin_terms.list,
                           ad.desc_origin_terms.num,
                           one_case.ad.desc_terms.list,
                           one_case.ad.desc_terms.num,
                           uint32_t);
    get_uniq(reg_data.meta_data_.ad.desc_origin_terms,
             reg_data.meta_data_.ad.desc_merged_terms,
             reg_data);

    get_match_feature(reg_data.meta_data_.ad.key_merged_terms, 
                      reg_data.meta_data_.ad.desc_merged_terms,
                      reg_data.meta_data_.ad.key_match_desc,
                      reg_data.meta_data_.ad.key_nomatch_desc,
                      reg_data.meta_data_.ad.desc_nomatch_key,  
                      reg_data);

    META_ONECASE_CP_LIST_D(ad.lp_terms.list,
                       ad.lp_terms.num,
                       one_case.ad.lp_terms.list,
                       one_case.ad.lp_terms.num,
                       uint32_t);
    get_match_feature(reg_data.meta_data_.ad.title_merged_terms, 
                      reg_data.meta_data_.ad.lp_terms,
                      reg_data.meta_data_.ad.title_match_lp,
                      reg_data.meta_data_.ad.title_nomatch_lp,
                      reg_data.meta_data_.ad.lp_nomatch_title,  
                      reg_data);
    get_match_feature(reg_data.meta_data_.ad.desc_merged_terms, 
                      reg_data.meta_data_.ad.lp_terms,
                      reg_data.meta_data_.ad.desc_match_lp,
                      reg_data.meta_data_.ad.desc_nomatch_lp,
                      reg_data.meta_data_.ad.lp_nomatch_desc,  
                      reg_data);
                   
    META_ONECASE_CP_LIST_D(ad.lp_topic.list,                       
                       ad.lp_topic.num,
                       one_case.ad.lp_topic.list,
                       one_case.ad.lp_topic.num,
                       uint32_t);
    

#ifdef FEATURE_DEBUG_
    char buffer[65536] = {0};
    uint32_t offset = snprintf(buffer, sizeof(buffer), "%016lx\t%u\tkeyTitleMergedTerms",
                               reg_data.meta_data_.pv.pvid, 0);
    for (uint32_t i = 0; i < reg_data.meta_data_.ad.keytitle_merged_terms.num; i++)
    {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\t%u",
                           reg_data.meta_data_.ad.keytitle_merged_terms.list[i]);
    }
    ADR_INFO("FD\t%s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    offset = snprintf(buffer, sizeof(buffer), "%016lx\t%u\tKeyMergedTerms",
                      reg_data.meta_data_.pv.pvid, 0);
    for (uint32_t i = 0; i < reg_data.meta_data_.ad.key_merged_terms.num; i++)
    {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\t%u",
                           reg_data.meta_data_.ad.key_merged_terms.list[i]);
    }
    ADR_INFO("FD\t%s\n", buffer);
    ADR_INFO("FD\t%016lx\t%u\tAdCategory\t%u\n",
             reg_data.meta_data_.pv.pvid, 0,
             reg_data.meta_data_.ad.category);
    //ADR_INFO("FD\t%016lx\t%u\tAdShowURL\t%s\t%016lx\n",
    //         reg_data.meta_data_.pv.pvid, 0,
    //         reg_data.meta_data_.ad.showurl_domain,
    //         reg_data.meta_data_.ad.showurl_domain_md5);
#endif

    return 0;
}

int MetaFeatureManager::import_onecase_ad_query_match(const OneCase & one_case, RegressionData& reg_data) const
{
    //META_ONECASE_CP_ONE(match.pos, one_case.visit.pos);
    //META_ONECASE_CP_ONE(match.match_type, one_case.visit.ad_match_type);
    get_match_feature(reg_data.meta_data_.query.query_merged_terms,
                      reg_data.meta_data_.ad.key_merged_terms,
                      reg_data.meta_data_.match.query_key_match,
                      reg_data.meta_data_.match.query_key_nomatch,
                      reg_data.meta_data_.match.key_query_nomatch,
                      reg_data);
    get_match_feature(reg_data.meta_data_.query.query_merged_terms,
                      reg_data.meta_data_.ad.keytitle_merged_terms,
                      reg_data.meta_data_.match.query_keytitle_match,
                      reg_data.meta_data_.match.query_keytitle_nomatch,
                      reg_data.meta_data_.match.keytitle_query_nomatch,
                      reg_data);

#ifdef FEATURE_DEBUG_
    char buffer[65536] = {0};
    uint32_t offset = 0;
    // query_keytitle_match
    offset = snprintf(buffer, sizeof(buffer), "%016lx\t%u\tQueryKeyTitleMatch",
                      reg_data.meta_data_.pv.pvid, 0);
    for (uint32_t i = 0; i < reg_data.meta_data_.match.query_keytitle_match.num; i++)
    {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\t%u",
                           reg_data.meta_data_.match.query_keytitle_match.list[i]);
    }
    ADR_INFO("FD\t%s\n", buffer);
    // query_keytitle_nomatch
    memset(buffer, 0, sizeof(buffer));
    offset = snprintf(buffer, sizeof(buffer), "%016lx\t%u\tQueryKeyTitleNomatch",
                      reg_data.meta_data_.pv.pvid, 0);
    for (uint32_t i = 0; i < reg_data.meta_data_.match.query_keytitle_nomatch.num; i++)
    {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\t%u",
                           reg_data.meta_data_.match.query_keytitle_nomatch.list[i]);
    }
    ADR_INFO("FD\t%s\n", buffer);
    // keytitle_query_nomatch
    memset(buffer, 0, sizeof(buffer));
    offset = snprintf(buffer, sizeof(buffer), "%016lx\t%u\tKeyTitleQueryNomatch",
                      reg_data.meta_data_.pv.pvid, 0);
    for (uint32_t i = 0; i < reg_data.meta_data_.match.keytitle_query_nomatch.num; i++)
    {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\t%u",
                           reg_data.meta_data_.match.keytitle_query_nomatch.list[i]);
    }
    ADR_INFO("FD\t%s\n", buffer);
#endif

    return 0;
}

int MetaFeatureManager::import_onecase_visit_user(const OneCase & one_case, RegressionData& reg_data) const
{
    /// user
    /*
    META_ONECASE_CP_LIST_D(user.user_id, user.user_id_space, one_case.visit.user_id, strlen(one_case.visit.user_id) + 1, char);
    META_ONECASE_CP_ONE(user.user_info, one_case.user_info);
    process_user_info(one_case, reg_data);
    get_uniq(reg_data.meta_data_.user.session_query_terms,
             reg_data.meta_data_.user.session_query_merged_terms,
             reg_data);
    get_uniq(reg_data.meta_data_.user.last_query_terms,
             reg_data.meta_data_.user.last_query_merged_terms,
             reg_data);
    // user last query
    if (reg_data.meta_data_.user.user_info->user_session.query_list.size() > 0)
    {
        META_ONECASE_CP_LIST_D(user.last_query_str, user.last_query_space,
                               reg_data.meta_data_.user.user_info->user_session.query_list[0].query.c_str(),
                               reg_data.meta_data_.user.user_info->user_session.query_list[0].query.length() + 1,
                               char);
    }
    else
    {
        reg_data.meta_data_.user.last_query_str = "";
        reg_data.meta_data_.user.last_query_space = 0;
    }
    get_match_feature(reg_data.meta_data_.user.session_query_merged_terms,
                      reg_data.meta_data_.query.query_merged_terms,
                      reg_data.meta_data_.user.sessionQuery_currentQuery_match,
                      reg_data.meta_data_.user.sessionQuery_currentQuery_nomatch,
                      reg_data.meta_data_.user.currentQuery_sessionQuery_nomatch,
                      reg_data);
    get_match_feature(reg_data.meta_data_.user.last_query_merged_terms,
                      reg_data.meta_data_.query.query_merged_terms,
                      reg_data.meta_data_.user.lastQuery_currentQuery_match,
                      reg_data.meta_data_.user.lastQuery_currentQuery_nomatch,
                      reg_data.meta_data_.user.currentQuery_lastQuery_nomatch,
                      reg_data);

    // user long
    META_ONECASE_CP_ONE(user.user_long_pv_num, one_case.user_info->user_long.user_long_pv_num);
    META_ONECASE_CP_ONE(user.user_long_ck_num, one_case.user_info->user_long.user_long_ck_num);
    META_ONECASE_CP_ONE(user.user_long_cate_pv, one_case.user_info->user_long.user_long_pv);
    META_ONECASE_CP_LIST_D(user.user_long_l1_cate.list, user.user_long_l1_cate.num, &one_case.user_info->user_long.user_long_l1_cate[0], one_case.user_info->user_long.user_long_l1_cate.size(), uint32_t);
    META_ONECASE_CP_ONE(user.user_long_best_l1_cate, one_case.user_info->user_long.user_long_best_l1_cate);
    META_ONECASE_CP_ONE(user.user_long_worst_l1_cate, one_case.user_info->user_long.user_long_worst_l1_cate);
    META_ONECASE_CP_LIST_D(user.user_long_lda.list, user.user_long_lda.num, &one_case.user_info->user_long.user_long_lda[0], one_case.user_info->user_long.user_long_lda.size(),uint32_t);

    /// visit
    META_ONECASE_CP_ONE(visit.ip, one_case.visit.ip);
    META_ONECASE_CP_ONE(visit.region_province, one_case.visit.region);
    META_ONECASE_CP_ONE(visit.service_type, one_case.visit.service_type);
    META_ONECASE_CP_LIST_D(visit.pid, visit.pid_space, one_case.visit.pid, strlen(one_case.visit.pid) + 1, char);
    META_ONECASE_CP_ONE(visit.time, one_case.visit.time);
    reg_data.meta_data_.visit.hour = (((uint32_t)(one_case.visit.time / 3600)) % 24 + 8) % 24;*/

   
    return 0;
}

int MetaFeatureManager::import_onecase_show(const OneCase& one_case, RegressionData& reg_data) const
{
    reg_data.meta_data_.show.is_blue = (one_case.visit.area == 2) ? 1 : 0;
    reg_data.meta_data_.show.is_left = (one_case.visit.area == 1) ? 1 : 0;
    reg_data.meta_data_.show.is_crown = (one_case.ad.crown != 0) ? one_case.ad.crown : 0;
    reg_data.meta_data_.show.no_crown = (one_case.ad.crown == 0) ? 1 : 0;
    return 0;
}


void MetaFeatureManager::take_action(bool is_query_related, const MetaFeatureAction & feature_action, RegressionData& reg_data) const
{
    uint32_t i;
    for ( i = 0; i < feature_action.feature_list.size(); i++)
    {
        const MetaFeatureClassInfo & class_info = info_list_[feature_action.feature_list[i].feature_offset];
        if (is_query_related && class_info.flag == METAFEATURE_FLAG_QUERY || !is_query_related && class_info.flag != METAFEATURE_FLAG_QUERY)
        {
            if (class_info.num_offset != 0)
            {
                //特征非定长，保存在统一缓冲区
                uint32_t * item_num = (uint32_t *)((char *)&reg_data.meta_data_ + class_info.num_offset);
                //iterm_num为特征item数量限制，返回值为实际数量
                uint32_t limit_item_num = (reg_data.pool_size_ - (reg_data.pool_top_ - reg_data.pool_)) / class_info.unit_size;
                *item_num = 0;
#ifdef FEATURE_TIME_DEBUG_
                struct timespec begin, end;
                clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
#endif
                feature_action.feature_list[i].feature_ptr->get_feature(reg_data.meta_data_, limit_item_num,  reg_data.pool_top_, item_num);
#ifdef FEATURE_TIME_DEBUG_
                clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
                uint64_t time = (end.tv_sec - begin.tv_sec) * 1000000000 + (end.tv_nsec - begin.tv_nsec);
                pthread_mutex_lock(&time_lock);
                meta_feature_time_list_[class_info.offset] += time / 1000;
                pthread_mutex_unlock(&time_lock);
#endif
                *(void**)((char*)&reg_data.meta_data_ + class_info.offset) = reg_data.pool_top_;
                reg_data.pool_top_ += *item_num * info_list_[feature_action.feature_list[i].feature_offset].unit_size;
            }
            else
            {
#ifdef FEATURE_TIME_DEBUG_
                struct timespec begin, end;
                clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
#endif
                //特征定长，直接计算
                feature_action.feature_list[i].feature_ptr->get_feature(reg_data.meta_data_, 1, (char*) & (reg_data.meta_data_) + class_info.offset, NULL);
#ifdef FEATURE_TIME_DEBUG_
                clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
                uint64_t time = (end.tv_sec - begin.tv_sec) * 1000000000 + (end.tv_nsec - begin.tv_nsec);
                pthread_mutex_lock(&time_lock);
                meta_feature_time_list_[class_info.offset] += time / 1000;
                pthread_mutex_unlock(&time_lock);
#endif
            }
        }
    }
}
int MetaFeatureManager::generate_meta_features(const OneCase & one_case, const MetaFeatureAction & feature_action, RegressionData& reg_data) const
{
    bool query_valid = false;
    if (!is_init_)
    {
        ADR_ERROR("MetaFeatureManager Not proplely Initialed!\n");
        return -1;
    }
    if (!reg_data.is_inited())
    {
        ADR_ERROR("MetaData Not propley Inited!\n");
        return -1;
    }
    //判断query一致的话，query相关特征不重新计算，否则重新计算query
    if ((feature_action.action_flag & METAFEATURE_FLAG_QUERY) && reg_data.meta_data_.query.query_str != NULL && (one_case.visit.pvid == reg_data.meta_data_.pv.pvid))
    {
        reg_data.pool_top_ = reg_data.pool_query_ptr_;
        query_valid = true;
    }
    else
    {
        reg_data.clear_meta();
    }
#ifdef FEATURE_TIME_DEBUG_
    pthread_mutex_lock(&time_lock);
    calc_times_ ++;
    pthread_mutex_unlock(&time_lock);
    struct timespec begin, end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
#endif
    META_ONECASE_CP_ONE(pv.pvid, one_case.visit.pvid);
#ifdef FEATURE_TIME_DEBUG_
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    uint64_t time = (end.tv_sec - begin.tv_sec) * 1000000000 + (end.tv_nsec - begin.tv_nsec);
    pthread_mutex_lock(&time_lock);
    meta_feature_time_list_[0] += time / 1000;
    pthread_mutex_unlock(&time_lock);
#endif
    if ((feature_action.action_flag & METAFEATURE_FLAG_QUERY) && !query_valid)
    {
#ifdef FEATURE_TIME_DEBUG_
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
#endif
        import_onecase_query(one_case, reg_data);
#ifdef FEATURE_TIME_DEBUG_
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
        uint64_t time = (end.tv_sec - begin.tv_sec) * 1000000000 + (end.tv_nsec - begin.tv_nsec);
        pthread_mutex_lock(&time_lock);
        meta_feature_time_list_[1] += time / 1000;
        pthread_mutex_unlock(&time_lock);
#endif
        take_action(true, feature_action, reg_data);
        reg_data.pool_query_ptr_ = reg_data.pool_top_;
    }
    if (feature_action.action_flag & METAFEATURE_FLAG_AD)
    {
#ifdef FEATURE_TIME_DEBUG_
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
#endif
        import_onecase_ad(one_case, reg_data);
#ifdef FEATURE_TIME_DEBUG_
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
        uint64_t time = (end.tv_sec - begin.tv_sec) * 1000000000 + (end.tv_nsec - begin.tv_nsec);
        pthread_mutex_lock(&time_lock);
        meta_feature_time_list_[2] += time / 1000;
        pthread_mutex_unlock(&time_lock);
#endif
    }
    if (feature_action.action_flag & METAFEATURE_FLAG_AD_MATCH)
    {
#ifdef FEATURE_TIME_DEBUG_
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
#endif
        import_onecase_ad_query_match(one_case, reg_data);
#ifdef FEATURE_TIME_DEBUG_
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
        uint64_t time = (end.tv_sec - begin.tv_sec) * 1000000000 + (end.tv_nsec - begin.tv_nsec);
        pthread_mutex_lock(&time_lock);
        meta_feature_time_list_[3] += time / 1000;
        pthread_mutex_unlock(&time_lock);
#endif
    }
   
    /*
    if (feature_action.action_flag & METAFEATURE_FLAG_VISIT_USER)
    {
#ifdef FEATURE_TIME_DEBUG_
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
#endif
        import_onecase_visit_user(one_case, reg_data);
#ifdef FEATURE_TIME_DEBUG_
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
        uint64_t time = (end.tv_sec - begin.tv_sec) * 1000000000 + (end.tv_nsec - begin.tv_nsec);
        pthread_mutex_lock(&time_lock);
        meta_feature_time_list_[4] += time / 1000;
        pthread_mutex_unlock(&time_lock);
#endif
    } */
    /*
    if (feature_action.action_flag & METAFEATURE_FLAG_SHOW)
    {
#ifdef FEATURE_TIME_DEBUG_
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
#endif
        import_onecase_show(one_case, reg_data);
#ifdef FEATURE_TIME_DEBUG_
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
        uint64_t time = (end.tv_sec - begin.tv_sec) * 1000000000 + (end.tv_nsec - begin.tv_nsec);
        pthread_mutex_lock(&time_lock);
        meta_feature_time_list_[5] += time / 1000;
        pthread_mutex_unlock(&time_lock);
#endif
    } */
    take_action(false, feature_action, reg_data);
    return 0;
}
int MetaFeatureManager::get_meta_data_info(uint32_t feature_offset, RegressionData& reg_data, MetaFeatureDataInfo & info) const
{
    info.size = info_list_[feature_offset].unit_size;
    if (info_list_[feature_offset].num_offset == 0)
    {
        info.list = (char *) & (reg_data.meta_data_) + info_list_[feature_offset].offset;
        info.num = 1;
    }
    else
    {
        info.list = *(void**)((char *) & (reg_data.meta_data_) + info_list_[feature_offset].offset);
        info.num = *(uint32_t *)((char *) & (reg_data.meta_data_) + info_list_[feature_offset].num_offset);
    }
    return 0;
}

void MetaFeatureManager::get_match_feature(const Uint32List src_list, const Uint32List matching_list1, const Uint32List matching_list2,
                                           Uint32List& match_dest_list, Uint32List& src_nonmatch_list, Uint32List& matching_nonmatch_list,
                                           RegressionData& reg_data) const
{
    uint32_t matching_num = matching_list1.num + matching_list2.num;
    uint32_t matching_list[matching_num];
    memcpy(matching_list, matching_list1.list, matching_list1.num * sizeof(uint32_t));
    memcpy(matching_list + matching_list1.num, matching_list2.list, matching_list2.num * sizeof(uint32_t));
    std::sort (matching_list, matching_list + matching_num);
    if (matching_num != 0)
    {
        uint32_t pre_idx = 0;
        for (uint32_t idx = 1; idx < matching_num; idx++)
        {
            if (matching_list[pre_idx] != matching_list[idx])
            {
                matching_list[++pre_idx] = matching_list[idx];
            }
        }
        matching_num = pre_idx + 1;
    }
    Uint32List tmp_list;
    tmp_list.list = matching_list;
    tmp_list.num = matching_num;
    get_match_feature(src_list, tmp_list, match_dest_list, src_nonmatch_list, matching_nonmatch_list, reg_data);
}

void MetaFeatureManager::get_match_feature(const Uint32List src_list, const Uint32List matching_list,
                                           Uint32List& match_dest_list, Uint32List& src_nonmatch_list, Uint32List& matching_nonmatch_list,
                                           RegressionData& reg_data) const
{
    uint32_t qcopy[src_list.num + 1];
    uint32_t tcopy[matching_list.num + 1];
    uint32_t nonmatch_src[src_list.num + 1];
    uint32_t nonmatch_matching[matching_list.num + 1];
    uint32_t nonmatch_src_num = 0, nonmatch_matching_num = 0;
    match_dest_list.list = (uint32_t*) (reg_data.pool_top_);
    match_dest_list.num = 0;
    //此处的输入两个list都必须已经升序排序，并Uniq过，否则程序会出错。
    memcpy(qcopy, src_list.list, src_list.num * sizeof(uint32_t));
    qcopy[src_list.num] = 0xFFFFFFFF;
    memcpy(tcopy, matching_list.list, matching_list.num * sizeof(uint32_t));
    tcopy[matching_list.num] = 0xFFFFFFFF;
    uint32_t qc_ind = 0, tc_ind = 0;
    while (qcopy[qc_ind] < 0xFFFFFFFF || tcopy[tc_ind] < 0xFFFFFFFF)
    {
        if (qcopy[qc_ind] < tcopy[tc_ind])
        {
            nonmatch_src[nonmatch_src_num] = qcopy[qc_ind];
            nonmatch_src_num ++;
            qc_ind ++;
        }
        else if (qcopy[qc_ind] > tcopy[tc_ind])
        {
            nonmatch_matching[nonmatch_matching_num] = tcopy[tc_ind];
            nonmatch_matching_num ++;
            tc_ind ++;
        }
        else
        {
            match_dest_list.list[match_dest_list.num] = qcopy[qc_ind];
            match_dest_list.num ++;
            qc_ind ++;
            tc_ind ++;
        }
    }
    reg_data.pool_top_ += sizeof(uint32_t) * match_dest_list.num;
    src_nonmatch_list.list = (uint32_t*) (reg_data.pool_top_);
    src_nonmatch_list.num = nonmatch_src_num;
    memcpy(src_nonmatch_list.list, nonmatch_src, sizeof(uint32_t) * nonmatch_src_num);
    reg_data.pool_top_ += sizeof(uint32_t) * src_nonmatch_list.num;
    matching_nonmatch_list.list = (uint32_t*) (reg_data.pool_top_);
    matching_nonmatch_list.num = nonmatch_matching_num;
    memcpy(matching_nonmatch_list.list, nonmatch_matching, sizeof(uint32_t) * nonmatch_matching_num);
    reg_data.pool_top_ += sizeof(uint32_t) * matching_nonmatch_list.num;
}

void MetaFeatureManager::get_uniq(Uint32List src_list, Uint32List& dest_list, RegressionData& reg_data) const
{
    dest_list.list = (uint32_t*)reg_data.pool_top_;
    dest_list.num = 0;
    if (src_list.num == 0)
    {
        return;
    }
    uint32_t tcopy[src_list.num];
    memcpy(tcopy, src_list.list, src_list.num * sizeof(uint32_t));
    std::sort(tcopy, tcopy + src_list.num);
    dest_list.list[0] = tcopy[0];
    dest_list.num ++;
    for (uint32_t i = 0; i < src_list.num; i++)
    {
        if (tcopy[i] != dest_list.list[dest_list.num - 1])
        {
            dest_list.list[dest_list.num] = tcopy[i];
            dest_list.num ++;
        }
    }
    reg_data.pool_top_ += dest_list.num * sizeof(uint32_t);
}

void MetaFeatureManager::fill_with_keyword(const char* src, uint32_t src_len,
                                           const char* insert, uint32_t insert_len,
                                           char*& dest, uint32_t& dest_space,
                                           RegressionData& reg_data) const
{
    dest_space = 0;
    dest = (char*)(reg_data.pool_top_);
    for (uint32_t i = 0; i < src_len; i++)
    {
        if (src[i] != '\1')
        {
            dest[dest_space] = src[i];
            dest_space ++;
        }
        else
        {
            memcpy(dest + dest_space, insert, sizeof(char) * insert_len);
            dest_space += insert_len;
        }
    }
    dest[dest_space] = '\0';
    dest_space ++;
    reg_data.pool_top_ += sizeof(char) * dest_space;
}

void MetaFeatureManager::fill_with_keyword(const Uint32List src_list, const Uint32List insert_list, Uint32List& desc_list, RegressionData& reg_data) const
{
    desc_list.num = 0;
    desc_list.list = (uint32_t*)reg_data.pool_top_;
    for (uint32_t i = 0; i < src_list.num; i++)
    {
        if (src_list.list[i] != 0)
        {
            desc_list.list[desc_list.num] = src_list.list[i];
            desc_list.num ++;
        }
        else
        {
            memcpy(desc_list.list + desc_list.num, insert_list.list, sizeof(uint32_t)*insert_list.num);
            desc_list.num += insert_list.num;
        }
    }
    reg_data.pool_top_ += sizeof(uint32_t) * desc_list.num;
}

void MetaFeatureManager::process_user_info(const OneCase& one_case, RegressionData& reg_data) const
{
    const uint32_t const_30m = 30 * 60;
    const vector<UserQueryInfo>* query_list = &one_case.user_info->user_session.query_list;
    const uint32_t query_size = query_list->size();
    const vector<UserPvInfo>* pv_list = &one_case.user_info->user_session.pv_list;
    const uint32_t pv_size = pv_list->size();

    vector<uint32_t> session_query_terms;
    vector<UserHistoryAdInfo> session_ads;
    reg_data.meta_data_.user.session_pv_num = 0;
    reg_data.meta_data_.user.session_adpv_num = 0;
    reg_data.meta_data_.user.session_adshow_num = 0;
    reg_data.meta_data_.user.session_adclick_num = 0;

    vector<uint32_t> last_query_terms;
    vector<UserHistoryAdInfo> last_query_ads;
    reg_data.meta_data_.user.last_query_adshow_num = 0;
    reg_data.meta_data_.user.last_query_adclick_num = 0;

    reg_data.meta_data_.user.pv_gap_from_last_click = 0;
    reg_data.meta_data_.user.is_cate3_clicked = false;
    map<uint32_t, uint32_t> last3pv_click_cate3_map;

    uint32_t query_idx = 0, pv_idx = 0;
    while (query_idx < query_size)
    {
        uint32_t time_gap = one_case.visit.time - (*query_list)[query_idx].time;
        if (time_gap > const_30m)
        {
            break;
        }

        // query terms
        uint32_t tmp_query_term_num = (*query_list)[query_idx].query_terms.size();
        for (uint32_t i = 0; i < tmp_query_term_num; i++)
        {
            session_query_terms.push_back((*query_list)[query_idx].query_terms[i]);
        }
        if (query_idx == 0)
        {
            for (uint32_t i = 0; i < tmp_query_term_num; i++)
            {
                last_query_terms.push_back((*query_list)[query_idx].query_terms[i]);
            }
        }
        // ads
        if ((pv_idx < pv_size)
                && ((*query_list)[query_idx].query == (*pv_list)[pv_idx].query)
                && ((*query_list)[query_idx].time == (*pv_list)[pv_idx].time))
        {
            uint32_t tmp_ad_num = (*pv_list)[pv_idx].ad_list.size();
            uint32_t tmp_click_num = 0;
            for (uint32_t i = 0; i < tmp_ad_num; i++)
            {
                UserHistoryAdInfo tmp_ad;
                tmp_ad.accid = (*pv_list)[pv_idx].ad_list[i].accid;
                tmp_ad.adid = (*pv_list)[pv_idx].ad_list[i].adid;
                tmp_ad.category = (*pv_list)[pv_idx].ad_list[i].category;
                tmp_ad.click = (*pv_list)[pv_idx].ad_list[i].is_click ? 1 : 0;
                tmp_ad.visit_time_gap = time_gap;
                tmp_ad.query_idx = query_idx;

                tmp_click_num += tmp_ad.click;
                session_ads.push_back(tmp_ad);
                if (query_idx == 0)
                {
                    last_query_ads.push_back(tmp_ad);
                }

                if (tmp_ad.click == 1)
                {
                    if (reg_data.meta_data_.user.pv_gap_from_last_click == 0)
                    {
                        reg_data.meta_data_.user.pv_gap_from_last_click = query_idx + 1;
                    }
                    if (tmp_ad.category == one_case.ad.category)
                    {
                        reg_data.meta_data_.user.is_cate3_clicked = true;
                    }
                    if (query_idx < 3)
                    {
                        last3pv_click_cate3_map[tmp_ad.category] += 1;
                    }
                }
            }
            reg_data.meta_data_.user.session_adpv_num += 1;
            reg_data.meta_data_.user.session_adshow_num += tmp_ad_num;
            reg_data.meta_data_.user.session_adclick_num += tmp_click_num;
            if (query_idx == 0)
            {
                reg_data.meta_data_.user.last_query_adshow_num += tmp_ad_num;
                reg_data.meta_data_.user.last_query_adclick_num += tmp_click_num;
            }
            pv_idx += 1;
        }
        reg_data.meta_data_.user.session_pv_num += 1;
        query_idx += 1;
    }

    // session_query_terms
    sort(session_query_terms.begin(), session_query_terms.end());
    uint32_t session_query_term_num = session_query_terms.size();
    reg_data.meta_data_.user.session_query_terms.num = session_query_term_num;
    reg_data.meta_data_.user.session_query_terms.list = (uint32_t*)(reg_data.pool_top_);
    uint32_t* p_session_query_term = reg_data.meta_data_.user.session_query_terms.list;
    for (uint32_t i = 0; i < session_query_term_num; i++)
    {
        p_session_query_term[i] = session_query_terms[i];
    }
    reg_data.pool_top_ += session_query_term_num * sizeof(uint32_t);
    // session_ads
    uint32_t session_ad_num = session_ads.size();
    reg_data.meta_data_.user.session_ads.num = session_ad_num;
    reg_data.meta_data_.user.session_ads.list = (UserHistoryAdInfo*)(reg_data.pool_top_);
    UserHistoryAdInfo* p_session_ad = reg_data.meta_data_.user.session_ads.list;
    for (uint32_t i = 0; i < session_ad_num; i++)
    {
        p_session_ad[i].accid = session_ads[i].accid;
        p_session_ad[i].adid = session_ads[i].adid;
        p_session_ad[i].category = session_ads[i].category;
        p_session_ad[i].click = session_ads[i].click;
        p_session_ad[i].visit_time_gap = session_ads[i].visit_time_gap;
        p_session_ad[i].query_idx = session_ads[i].query_idx;
    }
    reg_data.pool_top_ += session_ad_num * sizeof(UserHistoryAdInfo);
    // last_query_terms
    sort(last_query_terms.begin(), last_query_terms.end());
    uint32_t last_query_term_num = last_query_terms.size();
    reg_data.meta_data_.user.last_query_terms.num = last_query_term_num;
    reg_data.meta_data_.user.last_query_terms.list = (uint32_t*)(reg_data.pool_top_);
    uint32_t* p_last_query_term = reg_data.meta_data_.user.last_query_terms.list;
    for (uint32_t i = 0; i < last_query_term_num; i++)
    {
        p_last_query_term[i] = last_query_terms[i];
    }
    reg_data.pool_top_ += last_query_term_num * sizeof(uint32_t);
    // last_query_ads
    uint32_t last_query_ad_num = last_query_ads.size();
    reg_data.meta_data_.user.last_query_ads.num = last_query_ad_num;
    reg_data.meta_data_.user.last_query_ads.list = (UserHistoryAdInfo*)(reg_data.pool_top_);
    UserHistoryAdInfo* p_last_query_ad = reg_data.meta_data_.user.last_query_ads.list;
    for (uint32_t i = 0; i < last_query_ad_num; i++)
    {
        p_last_query_ad[i].accid = last_query_ads[i].accid;
        p_last_query_ad[i].adid = last_query_ads[i].adid;
        p_last_query_ad[i].category = last_query_ads[i].category;
        p_last_query_ad[i].click = last_query_ads[i].click;
        p_last_query_ad[i].visit_time_gap = last_query_ads[i].visit_time_gap;
        p_last_query_ad[i].query_idx = 0;
    }
    reg_data.pool_top_ += last_query_ad_num * sizeof(UserHistoryAdInfo);
    // last3pv_click_cate3_list
    reg_data.meta_data_.user.last3pv_click_cate3_list.num = last3pv_click_cate3_map.size();
    reg_data.meta_data_.user.last3pv_click_cate3_list.list = (uint32_t*)(reg_data.pool_top_);
    uint32_t* p_last3pv_click_cate3_list = reg_data.meta_data_.user.last3pv_click_cate3_list.list;
    uint32_t idx_last3pv_click_cate3_list = 0;
    for (map<uint32_t, uint32_t>::iterator iter = last3pv_click_cate3_map.begin(); iter != last3pv_click_cate3_map.end(); iter++)
    {
        p_last3pv_click_cate3_list[idx_last3pv_click_cate3_list++] = iter->first;
    }
    reg_data.pool_top_ += idx_last3pv_click_cate3_list * sizeof(uint32_t);

    return;
}

#ifdef FEATURE_TIME_DEBUG_
void MetaFeatureManager::get_time_debug(long*& time_list, uint32_t& time_num, uint32_t& calc_times)
{
    time_list = meta_feature_time_list_;
    time_num = sizeof(MetaFeatureData);
    calc_times = calc_times_;
}
#endif

