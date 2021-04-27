#ifndef CACHEMERE_EVICTION_LRU
#define CACHEMERE_EVICTION_LRU

#include <cassert>
#include <functional>
#include <list>
#include <map>

#include "cachemere/item.h"

namespace cachemere::policy {

/// @brief Least Recently Used (LRU) eviction policy.
/// @details Implemented internally using a linked list.
///          The keys are ordered from most-recently used to least-recently used.
///          Only stores references to keys kept alive by the cache.
/// @tparam Key The type of the keys used to identify items in the cache.
/// @tparam Value The type of the values stored in the cache.
template<typename Key, typename Value> class EvictionLRU
{
private:
    using KeyRef    = std::reference_wrapper<const Key>;
    using KeyRefIt  = typename std::list<KeyRef>::iterator;
    using KeyRefMap = std::map<KeyRef, KeyRefIt, std::less<const Key>>;

public:
    using CacheItem = cachemere::Item<Key, Value>;

    /// @brief Iterator for iterating over cache items in the order they should be
    ///        evicted.
    class VictimIterator
    {
    public:
        using KeyRefReverseIt = typename std::list<KeyRef>::const_reverse_iterator;

        VictimIterator(const KeyRefReverseIt& p_Iterator);

        const Key&      operator*() const;
        VictimIterator& operator++();
        VictimIterator  operator++(int);
        bool            operator==(const VictimIterator& other) const;
        bool            operator!=(const VictimIterator& other) const;

    private:
        KeyRefReverseIt m_iterator;
    };

    /// @brief Insertion event handler.
    /// @details Inserts the provided item at the front of the list.
    /// @param item The item that has been inserted in cache.
    void on_insert(const CacheItem& item);

    /// @brief Update event handler.
    /// @details Moves the provided item to the front of the list.
    /// @param item The item that has been updated in the cache.
    void on_update(const CacheItem& item);

    /// @brief Cache hit event handler.
    /// @details Moves the provided item at the front of the list.
    /// @param item The item that has been hit.
    void on_cache_hit(const CacheItem& item);

    /// @brief Eviction event handler.
    /// @details Removes the item at the back of the list - ensuring it has the provided key.
    /// @param item The key of the item that was evicted.
    void on_evict(const Key& item);

    /// @brief Get an iterator to the first item that should be evicted.
    /// @details Considering that the keys are ordered internally from most-recently used
    ///          to least-recently used, this iterator will effectively walk the internal
    ///          structure backwards.
    /// @return An item iterator.
    [[nodiscard]] VictimIterator victim_begin() const;

    /// @brief Get an end iterator.
    /// @return The end iterator.
    [[nodiscard]] VictimIterator victim_end() const;

private:
    std::list<KeyRef> m_keys;
    KeyRefMap         m_nodes;
};

}  // namespace cachemere::policy

#include "eviction_lru.hpp"

#endif