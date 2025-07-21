#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <deque>
#include <memory>
#include <unordered_map>
#include <utility>

#include "../containers/sparse_set.hpp"
#include "../containers/sparse_map.hpp"
#include "../containers/sparse_map_wrapper.hpp"
#include "entity.h"
#include "type.hpp"
//#include "view.hpp"

namespace ecs {

/*! \class registry
 *
 */
class registry
{
public:
    registry() = default;

    ~registry() {
        m_components.clear();
    }

    // disable copy
    registry(const registry&) = delete;
    void operator=(const registry&) = delete;
    
    /* Entity functions */
     
    /*! \brief Create new entity
     *  
     *  \returns Created entity
     */
    entity_type create() {
        // WARNING: Not thread safe
        // WARNING: Does not account for cases where no free entities are availiable
        entity_type entity;

        if (!m_unused_entities.empty()) {
            // check for collision?
            entity = m_unused_entities.front();
            m_unused_entities.pop_front();
        }

        else {
            // iterate until find non colliding entity,
            // required for cases where entity is created with
            // predefined ID
            while (m_entities.exists(m_next))m_next++;
            entity = m_next++;
        }

        m_entities.emplace(entity);
        return entity;
    }

    /*! \brief Create specific entity
     *
     *  \returns Created entity ID
     *  Created entity is not garuanteed to be equal to entity,
     *  uses default value if provided id already in use
     */
    entity_type create(const entity_type entity) {
        if (exists(entity))
            return entity;

        // check unused
        for (auto it = m_unused_entities.begin(); it != m_unused_entities.end(); it++)
        {
            if (*it == entity) {
                m_unused_entities.erase(it);
                break;
            }
        }

        m_entities.emplace(entity);
        return entity;
    }

    /*! \brief Destroys given entity and all associated components
     */
    void destroy(const entity_type entity) {
        // post warning?
        if (!exists(entity))
            return;

        clear(entity);
        m_entities.remove(entity);

        // entities greater than next are presumed to be free
        if (entity < m_next)
            m_unused_entities.push_back(entity);

        // could decrement and pop while unused.back == next - 1?
    }

    /*! \brief Destroy all entities and attatched components
     */
    void destroy_all() {

        // could add clear as virtual to sparse_map_base?
        for (auto it = m_entities.begin(); it != m_entities.end(); it++)
            clear(*it);

        // not deleting as iterating as when deleting swaps 
        m_entities.clear();

        // all values now free
        m_unused_entities.clear();
        m_next = 0;
    }

    /*! \brief Remove all components from entity without destroying
     */
    void clear(const entity_type entity) {
        if (!exists(entity))
            return;

        for (auto& comp_pool : m_components)
            comp_pool.second->remove(entity);
    }

    /*! \brief Checks if given entity exists within registry
     */
    bool exists(const entity_type entity) const {
        return m_entities.exists(entity);
    }

    /*! \brief Get number of entities in registry
     */
    uint32_t size() const {
        return m_entities.size();
    }

    /*! \brief Get reserved entity ID for null,
     * garuanteed to return false from exists, has and other similar functions
     */
    static constexpr entity_type tombstone() {
        return container::sparse_set::tombstone();
    }

    /* Entity iterators */

    typedef container::sparse_set::const_iterator const_entity_iterator;
    typedef container::sparse_set::iterator entity_iterator;

    const_entity_iterator entities_cbegin() const {
        return m_entities.cbegin();
    }

    const_entity_iterator entities_begin() const {
        return m_entities.cbegin();
    }

    entity_iterator entities_begin() {
        return m_entities.begin();
    }

    const_entity_iterator entities_cend() const {
        return m_entities.cend();
    }

    const_entity_iterator entities_end() const {
        return m_entities.cend();
    }

    entity_iterator entities_end() {
        return m_entities.end();
    }

    /* Component funcitons */

    /*! \brief Create new instance of Type for given entity, passing args to constructor
     */
    template<typename T, typename... Args>
    auto& emplace(const entity_type entity, Args&&... args)
    {
        create(entity);

        auto pool = get_component_pool<T>();

        return pool.emplace(entity, std::forward<Args>(args)...);
    }

    template<typename T>
    T& set(const entity_type entity, const T& component)
    {
        create(entity);

        auto pool = get_component_pool<T>();

        // runs default allocator if non-existant, slowdown?
        return (pool.get(entity).component = component);
    }

    /*! \brief Destroy attached component from entity
     */
    template<typename T>
    void remove(const entity_type entity)
    {
        const internal::type_info type_info = internal::type_info(std::in_place_type<T>);
        remove(entity, type_info);
    }

    void remove(const entity_type entity, const internal::type_info& type_info)
    {
        if (!exists(entity))
            return;

        auto pool = peek_component_pool(type_info);
        if (pool != nullptr)
            pool->remove(entity);
    }

    /*! \brief Fetch reference to component attached to entity
     *  \note Runs default constructor for component if non-existant
     */
    template<typename T>
    auto& get(const entity_type entity)
    {
        create(entity);
        return get_component_pool<T>().get(entity);
    }

    void* get(const entity_type entity, const internal::type_info& type_info)
    {
        create(entity);
        return get_component_pool(type_info)->get(entity);
    }

    template<typename T>
    auto* try_get(const entity_type entity)
    {
        const internal::type_info type_info = internal::type_info(std::in_place_type<T>);

        return reinterpret_cast<container::wrapped_sparse_map<T>*>(try_get(entity, type_info));
    }

    template<typename T>
    const auto* try_get(const entity_type entity) const
    {
        const internal::type_info type_info = internal::type_info(std::in_place_type<T>);

        return reinterpret_cast<const typename container::wrapped_sparse_map<T>::container_type*>(try_get(entity, type_info));
    }

    void* try_get(const entity_type entity, const internal::type_info& type_info)
    {
        if (!exists(entity))
            return nullptr;

        container::sparse_map* pool = peek_component_pool(type_info);

        return pool == nullptr ? nullptr : pool->try_get(entity);
    }

    const void* try_get(const entity_type entity, const internal::type_info& type_info) const
    {
        if (!exists(entity))
            return nullptr;

        const container::sparse_map* pool = peek_component_pool(type_info);

        return pool == nullptr ? nullptr : pool->try_get(entity);
    }


    // re-add multi check?

    /*! \brief Check if entity has given component type
     *
     *  \returns true if entity exists and satisfies all given component types
     */
    template<typename T>
    bool has(const entity_type entity) const
    {
        const internal::type_info type_info = internal::type_info(std::in_place_type<T>);
        return has(entity, type_info);
    }

    bool has(const entity_type entity, const internal::type_info& type_info) const
    {
        if (!exists(entity))
            return false;

        auto pool = peek_component_pool(type_info);
        return pool != nullptr && pool->exists(entity);
    }

    /*! \brief Get number of components of given type
     */
    template<typename T>
    uint32_t size() const
    {
        const internal::type_info type_info = internal::type_info(std::in_place_type<T>);

        return size(type_info);
    }

    uint32_t size(const internal::type_info& type_info) const
    {
        const container::sparse_map* pool = peek_component_pool(type_info);

        return pool == nullptr ? 0 : pool->size();
    }

    /*! \brief Create a view for iteration of all entities satisfying given types
     *
     *  \note Used for systems
     *  \note Views should not be stored, recreate for each singular use
     */
    //template<typename... Type>
    //view<Type...> create_view() {
    //    return view(get_component_pool<Type>()...);
    //}

    /*! \brief Get the pool storing a specific component
     */
    template<typename T>
    container::wrapped_sparse_map<T> get_pool() {
        return get_component_pool<T>();
    }

    container::sparse_map* get_pool(const internal::type_info& type_info) {
        return get_component_pool(type_info);
    }

    const container::sparse_map* try_get_pool(const internal::type_info& type_info) const {
        return peek_component_pool(type_info);
    }

    template<typename T>
    inline bool pool_exists() const {
        const internal::type_info type_info = internal::type_info(std::in_place_type<T>);
        return m_components.count(type_info);
    }

    inline bool pool_exists(const internal::type_info& type_info) const {
        return m_components.count(type_info);
    }

private:

    /*! \brief Get component pool for given type, creating if non-existant
     */
    template<typename T>
    container::wrapped_sparse_map<T> get_component_pool()
    {
        const internal::type_info type_info = 
            internal::type_info(std::in_place_type<T>);

        return container::wrapped_sparse_map<T>(get_component_pool(type_info));
    }

    container::sparse_map* get_component_pool(const internal::type_info& type_info)
    {
        if (!m_components.count(type_info))
            m_components.emplace(std::make_pair(type_info, std::make_unique<container::sparse_map>(type_info)));

        return m_components.at(type_info).get();
    }

    // NOT THREAD SAFE
    container::sparse_map* peek_component_pool(const internal::type_info& type_info)
    {
        if (!m_components.count(type_info))
            return nullptr;

        return m_components.at(type_info).get();
    }

    const container::sparse_map* peek_component_pool(const internal::type_info& type_info) const
    {
        if (!m_components.count(type_info))
            return nullptr;

        return m_components.at(type_info).get();
    }

    entity_type m_next = 0;
    std::deque<entity_type> m_unused_entities;

    container::sparse_set m_entities;

    // could do away with ptr, holdover from when fully templated
    std::unordered_map<internal::type_info, std::unique_ptr<container::sparse_map>> m_components;
};

} /* namespace ecs */

#endif /* end of include guard: REGISTRY_HPP */
