/*
 * Copyright © 2021 Jeremiah Ikosin
 * Distributed under the terms of the MIT license.
 */

#include "cxfixture.h"
#include <Muon/Muon.h>

// no seg-fault tests, because lru-cache is an internal structure with
// very precise use-case, and not meant to be used by external users.

void empty_lrucache_asserts(_cxml_lru_cache *cache){
    CHECK_EQ(cache->cache.capacity, 0);
    CHECK_EQ(cache->cache.count, 0);
    CHECK_NULL(cache->cache.entries);
    CHECK_TRUE(cxml_list_is_empty(&cache->cache.keys));
}

TEST(cxlrucache, cxml_cache_size){
    struct Data k, v;
    _cxml_lru_cache cache;
    _cxml_cache_init(&cache);

    _cxml_cache_put(&cache, &k, &v);
    CHECK_EQ(_cxml_cache_size(&cache), 1);

    _cxml_cache_put(&cache, &k, &v);
    CHECK_EQ(_cxml_cache_size(&cache), 1);

    _cxml_cache_put(&cache, &v, &v);
    CHECK_EQ(_cxml_cache_size(&cache), 2);

    _cxml_cache_put(&cache, &v, &v);
    CHECK_EQ(_cxml_cache_size(&cache), 2);

    _cxml_cache_free(&cache);
}

TEST(cxlrucache, cxml_cache_init){
    _cxml_lru_cache cache;
    _cxml_cache_init(&cache);
    empty_lrucache_asserts(&cache);
}

TEST(cxlrucache, cxml_cache_put){
    struct Data k, k2, k3, v, v2, v3;
    _cxml_lru_cache cache;
    _cxml_cache_init(&cache);

    _cxml_cache_put(&cache, &k, &v);
    _cxml_cache_put(&cache, &k2, &v2);
    CHECK_EQ(_cxml_cache_size(&cache), 2);
    CHECK_EQ(cxml_list_first(&cache.cache.keys), &k);
    CHECK_EQ(cxml_list_last(&cache.cache.keys), &k2);

    _cxml_cache_put(&cache, &k3, &v3);
    CHECK_EQ(cxml_list_last(&cache.cache.keys), &k3);
    CHECK_EQ(_cxml_cache_size(&cache), 3);

    _cxml_cache_free(&cache);
}

TEST(cxlrucache, cxml_cache_get){
    struct Data k, k2, k3, v, v2, v3;
    _cxml_lru_cache cache;
    _cxml_cache_init(&cache);

    _cxml_cache_put(&cache, &k, &v);
    _cxml_cache_put(&cache, &k2, &v2);

    CHECK_EQ(cxml_list_first(&cache.cache.keys), &k);

    void *d = _cxml_cache_get(&cache, &k);
    CHECK_EQ(d, &v);
    // &k is made recently accessed item.
    CHECK_EQ(cxml_list_last(&cache.cache.keys), &k);

    _cxml_cache_put(&cache, &k3, &v3);
    CHECK_EQ(cxml_list_last(&cache.cache.keys), &k3);

    d = _cxml_cache_get(&cache, &k2);
    CHECK_EQ(d, &v2);
    // &k2 is made recently accessed item.
    CHECK_EQ(cxml_list_last(&cache.cache.keys), &k2);
    // &k is currently at the top of the underlying list,
    // since it's the least recently used
    CHECK_EQ(cxml_list_first(&cache.cache.keys), &k);
    _cxml_cache_free(&cache);

}

TEST(cxlrucache, cxml_cache_free){
    struct Data k, v;
    _cxml_lru_cache cache;
    _cxml_cache_init(&cache);

    _cxml_cache_put(&cache, &k, &v);
    CHECK_EQ(_cxml_cache_size(&cache), 1);

    _cxml_cache_put(&cache, &v, &k);
    CHECK_EQ(_cxml_cache_size(&cache), 2);
    _cxml_cache_free(&cache);

    empty_lrucache_asserts(&cache);
}
