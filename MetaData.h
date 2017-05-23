/**
 *  Meta data
 *  @desc : define all the meta data that are used to generate the features for machine learning models. 
 *  @author : ylyang
 *  @email : yangyulian202@gmail.com
 *  @date : 2017-4-8
 *
*/

#ifndef _METADATA_H_
#define _METADATA_H_

namespace model
{
// meta data struct
typedef struct
{
    uint32_t * list;
    uint32_t num;
} Uint32List;
    
typedef struct
{
    uint64_t * list;
    uint32_t num;
} Uint64List;

// specify the meta data: should replace the following with the meta data in your case
struct QueryMetaInfo
{
    char* query_str;                                        ///< query
    uint64_t query_md5;                                     ///< MD5(query)
    Uint32List query_origin_terms;                          ///< query terms, original segmented result
    Uint32List query_selected_terms;                        ///< important query terms, sorted in ascending order + uniq

    uint32_t query_cluster;                                 ///< query_cluster
    uint32_t query_class;                                   ///< query_classify_category
    Uint64List query_ctr;                                   /// ctr in history

    uint32_t query_concept_id;                              ///< query_concept_id

    Uint32List query_topic;                                 ///topics the query belong
};

struct DocMetaInfo
{
    uint32_t desc_len;                                      ///< length of desc, high 16-bit for desc1, low 16-bit for desc2

    // title
    char* title_str;                                          ///< keyword
    uint64_t title_md5;                                       ///< MD5(key)
    Uint32List title_origin_terms;                            ///< key terms, original segmented result
    Uint32List title_selected_terms;                          ///< important key terms, sorted in ascending order + uniq

    // desc
    char* desc_str;
    Uint32List desc_origin_terms;
    Uint32List desc_selected_terms;

    // other info that may be useful
};

struct UserMetaInfo
{
    char* user_id;                                          ///< user_id

    Uint32List session_query_terms;                         ///< session: query terms
    Uint32List session_query_selected_terms;                ///< session: important query terms, sorted and uniq

    Uint32List last_query_terms;                            ///< last query: query terms
    Uint32List last_query_selected_terms;                   ///< last query: selected terms, sorted and uniq

    // other info that may be useful
};
    
struct MetaFeatureData
{                            
    QueryMetaInfo query;                             
    DocMetaInfo doc;                                                          
    UserMetaInfo user;      
};
}
#endif
