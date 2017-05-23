#ifndef _METAFEATUREDATA_H_
#define _METAFEATUREDATA_H_
#include "OneCase.h"
#include "LDA/LdaUtils.h"
#include <utility>

#define METAOFFSETOF(element) offsetof(MetaFeatureData, element)

namespace regression
{
typedef struct
{
    uint32_t first;
    uint32_t second;
} TermPair;

typedef struct
{
    TermPair * list;
    uint32_t num;
} TermPairList;

typedef struct
{
    uint32_t termid;
    uint32_t query_pos;
    uint32_t ad_pos;
} TermDoublePos;

typedef struct
{
    TermDoublePos * list;
    uint32_t num;
} TermDoublePosList;

typedef struct
{
    float * list;
    uint32_t num;
} FloatList;

typedef struct
{
    uint64_t * list;
    uint32_t num;
} Uint64List;

typedef struct
{
    std::pair<uint32_t, bool> *list;
    uint32_t num;
} SliceList;

typedef struct
{
    uint32_t accid;
    uint32_t adid;
    uint32_t category;
    uint32_t click;
    uint32_t visit_time_gap;
    uint32_t query_idx;
} UserHistoryAdInfo;

typedef struct
{
    UserHistoryAdInfo* list;
    uint32_t num;
} UserHistoryAdInfoList;

typedef struct
{
    topicmodel::TopicInfo* list;
    uint32_t num;
} TopicInfoList;

struct AD_MATCH_TYPE
{
    static const uint8_t EXACT_AD_MATCH = 1;
    static const uint8_t BROAD_AD_MATCH = 2;
};

struct PvMetaFeatureInfo
{
    uint64_t pvid;                                          ///< pvid
};

struct QueryMetaFeatureInfo
{
    char* query_str;                                        ///< query
    uint32_t query_space;                                   ///< strlen(query_str)+1
    uint64_t query_md5;                                     ///< MD5(query)
    Uint32List query_origin_terms;                          ///< query terms, original segmented result
    Uint32List query_merged_terms;                          ///< query terms, sorted in ascending order + uniq

    Uint32List query_selected_terms;                        ///< important query terms, sorted in ascending order + uniq

    uint32_t query_cluster;                                 ///< query_cluster
    uint32_t query_classify;                                ///< query_classify_category
    Uint64List query_ctr;                                   ///< pv num and ck num(30days): (query, not discretized)

    uint32_t query_concept_id;                              ///< query_concept_id

    TopicInfoList query_lda_distr;                          ///< query lda distribution
    Uint32List query_lda_topic;                             ///< query origin terms, major lda topic

    Uint32List query_content_region;                        ///< region in query, 2 most
    Uint32List query_lda_concurr_topic;                     ///< query lda concurrent topic

    IdWeightList query_extension;                           ///< query extension list
    Uint32List query_extension_selected;                    ///< query extension list, selected
};

struct AdMetaFeatureInfo
{
    uint32_t account_id;                                    ///< account_id
    uint32_t group_id;                                      ///< groupid
    uint32_t kid;                                           ///< kid
    uint32_t creative_id;                                   ///< cid
    uint32_t desc_len;                                      ///< length of desc, high 16-bit for desc1, low 16-bit for desc2

    char* key_str;                                          ///< keyword
    uint32_t key_space;                                     ///< strlen(key_str)+1
    uint64_t key_md5;                                       ///< MD5(key)
    Uint32List key_origin_terms;                            ///< key terms, original segmented result
    Uint32List key_merged_terms;                            ///< key terms, sorted in ascending order + uniq
    Uint32List key_selected_terms;                          ///< important key terms, sorted in ascending order + uniq

    char* title_str;                                        ///< title
    uint32_t title_space;                                   ///< strlen(title_str)+1
    Uint32List title_origin_terms;                          ///< title terms, original segmented result
    Uint32List title_merged_terms;                          ///< title terms, sorted in ascending order + uniq
    Uint32List title_selected_terms;                        ///< important title terms, sorted in ascending order + uniq

    Uint64List ad_term_cross;

    // desc
    char* desc_str;
    uint32_t desc_space;
    Uint32List desc_origin_terms;
    Uint32List desc_merged_terms;
    Uint32List desc_selected_terms;

    Uint32List key_match_desc;
    Uint32List key_nomatch_desc;
    Uint32List desc_nomatch_key;

    Uint32List title_match_lp;
    Uint32List title_nomatch_lp;
    Uint32List lp_nomatch_title;

    Uint32List desc_match_lp;
    Uint32List lp_nomatch_desc;
    Uint32List desc_nomatch_lp;

    Uint32List key_match_title;                             ///< key match title
    Uint32List key_nomatch_title;                           ///< key nomatch title
    Uint32List title_nomatch_key;                           ///< title nomatch key

    char* keytitle_str;                                     ///< KeyTitle: keyword + title
    uint32_t keytitle_space;                                ///< strlen(keytitle_str)+1
    uint64_t keytitle_md5;                                  ///< Md5(keyTitle_str)
    Uint32List keytitle_origin_terms;                       ///< KeyTitle terms, original segmented result
    Uint32List keytitle_merged_terms;                       ///< KeyTitle terms, sorted in ascending order + uniq

    Uint32List keytitle_selected_terms;                     ///< import keytitle terms, sorted in ascending order + uniq

    uint32_t category;                                      ///< category
    uint32_t category_L1;                                   ///< category: Level 1
    uint32_t kwd_classify;                                  ///< kwd_classify_category
    char* showurl_domain;                                   ///< domain of url
    uint32_t showurl_domain_space;                          ///< strlen(showurl_domain)+1
    uint64_t showurl_domain_md5;                            ///< MD5(showurl_domain)

    TopicInfoList ad_lda_distr;
    Uint32List ad_lda_topic;                                ///< keyword, major lda topic

    Uint32List ad_lda_concurr_topic;                        ///< keyword, concurrent lda topic
    Uint32List ad_content_region;                           ///< region in KeyTitle(term first, if none, consider phone number), 2 most
    Uint32List ad_concurr_cate;                             ///< ad concurrent cate_L1

    IdWeightList key_extension;                             ///< key extension list
    Uint32List key_extension_selected;                      ///< key extension list, selected

    Uint64List query_accid_ctr;                             ///< pv num and ck num(30 days): query_accid, not discretized

    // title and desc topic
    TopicInfoList title_lda_distr;
    TopicInfoList desc_lda_distr;
    Uint32List title_lda_topic;
    Uint32List desc_lda_topic;  

    float load_time;
    float mainpage_load_time;
    float invalid_resource_ratio;
    float invalid_pic_ratio;
    float page_rank;

    Uint32List landingpage_quality;

    Uint32List lp_terms;
    Uint32List lp_topic;

    Uint32List mouse_type_list;
    Uint32List mouse_num_list;
    Uint32List mouse_time_list;
    Uint32List mouse_distance_list;

};

struct MatchMetaFeatureInfo
{
    uint8_t pos;                                            ///< position
    uint32_t match_type;                                    ///< 1: original match, 2: broad match

    Uint32List query_key_match;                             ///< (source:merged_terms) match
    Uint32List query_key_nomatch;                           ///< (source:merged_terms) nomatch: query-key
    Uint32List key_query_nomatch;                           ///< (source:merged_terms) nomatch: key-title

    Uint32List query_keytitle_match;                        ///< (source:merged_terms) match
    Uint32List query_keytitle_nomatch;                      ///< (source:merged_terms) nomatch: query-KeyTitle
    Uint32List keytitle_query_nomatch;                      ///< (source:merged_terms) nomatch: KeyTitle-query

    Uint64List query_keytitle_synonym_pair_all;             ///< synonym pair: query.query_merged_terms * ad.keytitle_merged_terms
    TermPairList query_keytitle_synonym_pair_nomatch;       ///< synonym pair: match.query_keytitle_nomatch * match.keytitle_query_nomatch
    Uint32List query_synonym_hit;
    Uint32List query_synonym_not_hit;
    Uint32List keytitle_synonym_hit;
    Uint32List keytitle_synonym_not_hit;

    uint64_t query_key_match_whole;                         ///< MD5(match.query_key_match)
    uint64_t query_keytitle_match_whole;                    ///< MD5(match.query_keytitle_match)

    // selected
    Uint32List query_keytitle_match_selected;               ///< select important terms from query_keytitle_match
    Uint32List query_keytitle_nomatch_selected;             ///< select important terms from query_keytitle_nomatch
    Uint32List keytitle_query_nomatch_selected;             ///< select important terms from keytitle_query_nomatch

    TermPairList query_keytitle_synonym_pair_selected;      ///< synonym pair: query_keytitle_nomatch_selected * keytitle_query_nomatch_selected
    Uint32List query_synonym_hit_term_selected;
    Uint32List query_synonym_not_hit_term_selected;
    Uint32List keytitle_synonym_hit_term_selected;
    Uint32List keytitle_synonym_not_hit_term_selected;

    Uint32List query_match_ad_lda_topic;                    ///< lda topic: query match ad
    Uint32List query_nomatch_ad_lda_topic;                  ///< lda topic: query nomatch ad
    Uint32List ad_nomatch_query_lda_topic;                  ///< lda topic: ad nomatch query
    uint32_t query_ad_lda_cosine_sim;                       ///< cosine(query_lda, ad_lda)

    Uint32List query_match_key_extension;                   ///< query_extension match key_extension
    Uint32List query_nomatch_key_extension;                 ///< query_extension nomatch key_extension
    Uint32List key_nomatch_query_extension;                 ///< key_extension nomatch query_extension
    uint32_t query_key_extension_cosine_sim;                ///< cosine(query_extension, key_extension)

    Uint64List query_ad_cluster_cross;
    Uint64List query_ad_match_rank;

    Uint32List key_title_synonymmatch;
    Uint32List key_desc_synonymmatch;
    Uint32List key_lp_synonymmatch;
    Uint32List titledesc_lp_synonymmatch;
    
    Uint32List key_title_topicmatch;
    Uint32List key_title_topicnomatch;
    Uint32List key_desc_topicmatch;
    Uint32List key_desc_topicnomatch;
    Uint32List title_lp_topicmatch;
    Uint32List title_lp_topicnomatch;
    Uint32List desc_lp_topicmatch;
    Uint32List desc_lp_topicnomatch;

    // key-title, key-creative, key-lp match sim; title-lp, creative-lp match sim
    //Uint32List key_title_term_sim;
    //Uint32List key_cc_term_sim;
    //Uint32List key_lp_term_sim;
    //Uint32List titlecc_lp_term_sim;

    //Uint32List key_title_topic_sim;
    //Uint32List key_cc_topic_sim;
    //Uint32List key_lp_topic_sim;
    //Uint32List title_lp_topic_sim;
    //Uint32List desc_lp_topic_sim;
    Uint32List ad_lp_topic_sim;
};

struct VisitMetaFeatureInfo
{
    uint32_t ip;                                            ///< user ip
    uint32_t region;                                        ///< region(from ip)
    uint32_t region_province;                               ///< region_province: from sample

    char* pid;                                              ///< pid
    uint32_t pid_space;                                     ///< strlen(pid)+1

    uint32_t service_type;                                  ///< service_type

    uint32_t time;                                          ///< visit time
    uint32_t time_pattern;                                  ///< hour*holiday
    uint32_t hour;                                          ///< visit time: hour

    Uint64List pid_ctr;                                     ///< pv num and ck num(30 days): pid, not discretized
    Uint64List accid_region_ctr;                            ///< pv num and ck num(30 days): accid_region, not discretized

};

struct UserMetaFeatureInfo
{
    char* user_id;                                          ///< user_id, YYID|SUID
    uint32_t user_id_space;                                 ///< strlen(user_id)+1
    const UserInfo* user_info;                              ///< complete user_info from one_case

    Uint32List session_query_terms;                         ///< session: query terms
    Uint32List session_query_merged_terms;                  ///< session: query terms, merged
    Uint32List session_query_selected_terms;                ///< session: important query terms, sorted and uniq
    UserHistoryAdInfoList session_ads;                      ///< session: show ads

    Uint32List last_query_terms;                            ///< last query: query terms
    Uint32List last_query_merged_terms;                     ///< last query: query terms, merged
    Uint32List last_query_selected_terms;                   ///< last query: selected terms, sorted and uniq
    UserHistoryAdInfoList last_query_ads;                   ///< last query: ads

    uint32_t session_pv_num;                                ///< session: pv
    uint32_t session_adpv_num;                              ///< session: adpv
    uint32_t session_adshow_num;                            ///< session: ad num
    uint32_t session_adclick_num;                           ///< session: ad click num

    uint32_t last_query_adshow_num;                         ///< last query: ad num
    uint32_t last_query_adclick_num;                        ///< last query: ad click num

    TopicInfoList session_lda_distr;                        ///< session: lda distribution
    Uint32List session_lda_topic;                           ///< session: lda topic
    float session_lda_cosine_sim;                           ///< lda cosine similarity: session query vs. current query
    Uint32List session_common_lda_topic;                    ///< common lda topic: session query vs. current query

    TopicInfoList last_query_lda_distr;                     ///< last query: lda distribution
    Uint32List last_query_lda_topic;                        ///< last query: lda topic
    float last_query_lda_cosine_sim;                        ///< lda cosine similarity: last query vs. current query
    Uint32List last_query_lda_common;                       ///< common lda topic: lastQuery & currentQuery

    char* last_query_str;                                   ///< last query
    uint32_t last_query_space;                              ///< strlen(last_query_str) + 1
    Uint32List lastQuery_currentQuery_match;                ///< match: lastQuery & currentQuery
    Uint32List lastQuery_currentQuery_nomatch;              ///< nomatch: lastQuery - currentQuery
    Uint32List currentQuery_lastQuery_nomatch;              ///< nomatch: currentQuery - lastQuery
    uint32_t last_query_transform_type;                     ///< transform type: last query --> current query
    uint32_t last_query_flag;                               ///< last_query_transform_type * last_query_showad * last_query_click * common_term_num
    float lastQuery_currentQuery_cosine_sim;                ///< cosine(last_query, current_query)

    Uint32List sessionQuery_currentQuery_match;             ///< match: sessionQuery & currentQuery
    Uint32List sessionQuery_currentQuery_nomatch;           ///< nomatch: sessionQuery - currentQuery
    Uint32List currentQuery_sessionQuery_nomatch;           ///< nomatch: currentQuery - sessionQuery
    float sessionQuery_currentQuery_cosine_sim;             ///< cosine(session_query - current_query)

    Uint32List session_click_category;                      ///< user click in the session: category list
    Uint32List session_click_accid;                         ///< user click in the session: accid list
    Uint32List session_click_kid;                           ///< user click in the session: kid list

    uint32_t pv_gap_from_last_click;                        ///< pv gap from user's last click
    bool is_cate3_clicked;                                  ///< Is same category(Level 3) clicked ?
    Uint32List last3pv_click_cate3_list;                    ///< last 3 pv, clicked ads, cate3 list

    float user_long_pv_num;                                 ///< user long(30 days): pv num
    float user_long_ck_num;                                 ///< user long(30 days): ck num
    uint64_t user_long_ctr;                                 ///< user long(30 days): ctr after discretization
    uint32_t user_long_cate_pv;                             ///< user long(30 days): total pv of all categories
    Uint32List user_long_l1_cate;                           ///< user long(30 days): 1st-level category ctr list after discretization
    uint32_t user_long_best_l1_cate;                        ///< user long(30 days): most favourite 1st-level category
    uint32_t user_long_worst_l1_cate;                       ///< user long(30 days): least favourite 1st-level category
    Uint32List user_long_lda;                               ///< user long(30 days): lda list after discretization
};

struct ShowMetaFeatureInfo
{
    uint8_t is_blue;                                        ///< pvstyle: blue
    uint8_t is_left;                                        ///< pvstyle: left
    uint8_t is_crown;                                       ///< adstyle: crown online
    uint8_t no_crown;                                       ///< adstyle: crown offline
};

struct MetaFeatureData
{
    PvMetaFeatureInfo pv;                                   ///< Instance信息，非特征
    ShowMetaFeatureInfo show;                               ///< 样式相关特征，可以进入Cache，Online每个Instance须多个同时存在,Offline每个Instance只有1次
    QueryMetaFeatureInfo query;                             ///< 查询相关特征，每次Query只计算1次
    AdMetaFeatureInfo ad;                                   ///< 广告相关特征，可以进入Cache
    MatchMetaFeatureInfo match;                             ///< 查询-广告匹配相关特征，可以进入Cache
    VisitMetaFeatureInfo visit;                             ///< 访问相关特征，不能进入Cache
    UserMetaFeatureInfo user;                               ///< 用户相关特征，不能进入Cache
};
}
#endif
