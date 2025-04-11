#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <unordered_map>
#include <deque>
#include <utility>

#include "ecs/containers/sparse_set.hpp"
#include "ecs/containers/sparse_map.hpp"
#include "entity.h"
#include "type.hpp"
#include "view.hpp"

namespace ecs {

/*! \class registry
 *
 */
class registry
{
public:
    registry() = default;

    ~registry() {
        for (auto comp_pool : m_components)
            delete comp_pool.second;
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

        for (auto comp_pool : m_components)
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
    constexpr entity_type tombstone() const {
        return m_entities.tombstone();
    }

    /* Entity iterators */

    typedef container::sparse_set<>::const_iterator const_entity_iterator;
    typedef container::sparse_set<>::iterator entity_iterator;

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
    template<typename Type, typename... Args>
    Type& emplace(const entity_type entity, Args&&... args)
    {
        create(entity);

        container::sparse_map<Type>* pool = get_component_pool<Type>();

        return pool->emplace(entity, std::forward<Args>(args)...);
    }

    // necessary?
    template<typename Type>
    Type& set_emplace(const entity_type entity, const Type& component)
    {
        create(entity);

        container::sparse_map<Type>* pool = get_component_pool<Type>();

        return pool->emplace(entity, component);
    }

    template<typename Type>
    Type& set(const entity_type entity, const Type& component)
    {
        create(entity);

        container::sparse_map<Type>* pool = get_component_pool<Type>();

        // runs default allocator if non-existant, slowdown?
        return pool->get(entity) = component;
    }

    /*! \brief Destroy attached component from entity
     */
    template<typename Type>
    void remove(const entity_type entity)
    {
        if (!exists(entity))
            return;

        // only need to peek
        // as if pool non-existant there is nothing needed to do
        container::sparse_map<Type>* pool = peek_component_pool<Type>();

        if (pool != nullptr)
            pool->remove(entity);
    }

    /*! \brief Fetch reference to component attached to entity
     *  \note Runs default constructor for component if non-existant
     */
    template<typename Type>
    Type& get(const entity_type entity)
    {
        create(entity);

        container::sparse_map<Type>* pool = get_component_pool<Type>();

        return pool->get(entity);
    }

    /*! \brief Get copy of component from entity if exists
     *
     *  \note Only gets copy, functions different to other functions which return reference
     *
     *  \returns false if component non-existant
     */
    template<typename Type>
    const Type* try_get(const entity_type entity) const
    {
        if (!exists(entity))
            return nullptr;

        const container::sparse_map<Type>* pool = peek_component_pool<Type>();

        if (pool == nullptr)
            return nullptr;

        return pool->try_get(entity);
    }

    /*! \brief Check if entity has given component types
     *
     *  \returns true if entity exists and satisfies all given component types
     */
    template<typename Type, typename... TN>
    bool has(const entity_type entity) const {
        if (!exists(entity))
            return false;

        return has_a<Type, TN...>(entity);
    }

    /*! \brief Get number of components of given type
     */
    template<typename Type>
    uint32_t size() const {

        const container::sparse_map<Type>* pool = peek_component_pool<Type>();

        return pool == nullptr ? 0 : pool->size();
    }

    /*! \brief Create a view for iteration of all entities satisfying given types
     *
     *  \note Used for systems
     *  \note Views should not be stored, recreate for each singular use
     */
    template<typename... Type>
    view<Type...> create_view() {
        return view(get_component_pool<Type>()...);
    }

    /*! \brief Get the pool storing a specific component
     */
    template<typename Type>
    container::sparse_map<Type>* get_pool() {
        return get_component_pool<Type>();
    }

    template<typename Type>
    const container::sparse_map<Type>* try_get_pool() const {
        return peek_component_pool<Type>();
    }

private:

    /*! \brief Variadic component checking implementation
     */
    template<typename Type, typename... TN>
    inline std::enable_if_t<(sizeof...(TN) > 0), bool> has_a(const entity_type entity) const {
        return has_a<Type>(entity) && has_a<TN...>(entity);
    }

    /*! \brief End point for has_a function
     *
     *  Checks singular type and stops further recursion, without would not compile
     */
    template<typename Type, typename... TN>
    inline std::enable_if_t<(sizeof...(TN) == 0), bool> has_a(const entity_type entity) const {
        const container::sparse_map<Type>* pool = peek_component_pool<Type>();

        return pool != nullptr && pool->exists(entity);
    }

    typedef container::sparse_map_base* base_pool_type;

    /*! \brief Get component pool for given type, creating if non-existant
     */
    template<typename Type>
    container::sparse_map<Type>* get_component_pool()
    {
        using pool_type = container::sparse_map<Type>;

        const internal::type_info info = 
            internal::type_info(std::in_place_type_t<Type>());

        if (!m_components.count(info))
            m_components.emplace(std::make_pair(info, static_cast<base_pool_type>(new pool_type())));

        return dynamic_cast<pool_type*>(m_components[info]);
    }

    /*! \brief Get component pool for specific type if exists
     */
    template<typename Type>
    container::sparse_map<Type>* peek_component_pool()
    {
        using pool_type = container::sparse_map<Type>;

        const internal::type_info info = 
            internal::type_info(std::in_place_type_t<Type>());

        if (!m_components.count(info))
            return nullptr;

        return dynamic_cast<pool_type*>(m_components.at(info));
    }

    /*! \brief Get component pool for specific type if exists (const version)
     */
    template<typename Type>
    const container::sparse_map<Type>* peek_component_pool() const
    {
        using pool_type = container::sparse_map<Type>;

        const internal::type_info info = 
            internal::type_info(std::in_place_type_t<Type>());

        if (!m_components.count(info))
            return nullptr;

        return static_cast<pool_type*>(m_components.at(info));
    }

    entity_type m_next = 0;
    std::deque<entity_type> m_unused_entities;

    container::sparse_set<> m_entities;

    std::unordered_map<internal::type_info, base_pool_type> m_components;
};

} /* namespace ecs */

#endif /* end of include guard: REGISTRY_HPP */
