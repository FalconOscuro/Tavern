#ifndef VIEW_HPP
#define VIEW_HPP

#include "../containers/sparse_map.hpp"

namespace ecs {

// no longer standalone, requires registry alongside to function
class view final
{
public:

    friend class registry;

    view(const view&) = default;
    view& operator=(const view&) = default;

    ~view() = default;

    typedef std::vector<entity_type>::const_iterator iterator;

    size_t num_criteria() const {
        return m_criteria.size();
    }

    const core::type_info& get_criteria(const size_t i) {
        return m_criteria[i];
    }

    size_t size() const {
        return m_entities.size();
    }

    iterator begin() const {
        return m_entities.cbegin();
    }

    iterator end() const {
        return m_entities.cend();
    }

private:

    // take criteria separately in-case of non-existant criteria_pools
    // assume both orderd same
    view(const std::vector<std::pair<core::type_info, const container::sparse_map*>>& criteria_pools):
        m_criteria(), m_entities()
    {
        // throw error?
        if (criteria_pools.empty())
            return;

        m_criteria.reserve(criteria_pools.size());

        // find smallest pool
        // and fail if any pools are empty
        size_t smallest_index = 0;
        size_t smallest_pool_size = std::numeric_limits<size_t>::max();

        for (size_t i = 0; i < criteria_pools.size(); ++i)
        {
            m_criteria.emplace_back(criteria_pools[i].first);

            // account for non-existant pools
            const size_t pool_size = criteria_pools[i].second == nullptr ? 0 : criteria_pools[i].second->size();

            if (pool_size < smallest_pool_size)
            {
                smallest_index = i;
                smallest_pool_size = pool_size;
            }
        }

        // cannot be any matching entities as at least one criteria pool is empty/non-existant
        if (smallest_pool_size == 0)
            return;

        // find matching entities
        for (auto it = criteria_pools[smallest_index].second->cbegin(); it != criteria_pools[smallest_index].second->cend(); ++it)
        {
            const entity_type eid = *reinterpret_cast<const entity_type*>(it.data());

            // should always be true
            bool in_all_pools = eid != container::sparse_map::tombstone();

            for (size_t i = 0; i < criteria_pools.size(); ++i && in_all_pools)
            {
                if (i != smallest_index)
                    in_all_pools = criteria_pools[i].second->exists(eid);
            }

            if (in_all_pools)
                m_entities.push_back(eid);
        }
    }

    std::vector<core::type_info> m_criteria;
    std::vector<entity_type> m_entities;
};

} /* end of namespace ecs */

#endif /* end of define guard VIEW_HPP */
