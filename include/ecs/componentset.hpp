#pragma once

#include <vector>
#include <unordered_map>

#include "ecs/types.hpp"

namespace ecs {

// interface for component set
class IComponentSet {

public:
    // these are virtual because we will be using polymorphism to manage different component sets
    virtual ~IComponentSet() = default;
    virtual void entityDestroyed(EntityId entityId) = 0;
    virtual void clear() = 0;

};

// template class for component sparse set
template<typename ComponentType>
class ComponentSet : public IComponentSet {

public:
    void insert(EntityId entityId, ComponentType component);
    void remove(EntityId entityId);
    ComponentType& get(EntityId entityId);
    void entityDestroyed(EntityId entityId) override;
    void clear() override;
    bool has(EntityId entityId);
private:
    std::vector<ComponentType> components_; // dense array of components
    std::unordered_map<EntityId, size_t> entityToIndex_; // mapping from entity ID to index in the components_ vector
    std::vector<EntityId> indexToEntity_; // reverse mapping

    size_t size_ = 0; // how many components are currently stored in the set
};

template<typename ComponentType>
bool ComponentSet<ComponentType>::has(EntityId entityId) {
    return entityToIndex_.find(entityId) != entityToIndex_.end(); 
}

template<typename ComponentType>
void ComponentSet<ComponentType>::insert(EntityId entityId, ComponentType component) {
    if (entityToIndex_.find(entityId) != entityToIndex_.end()) {
        // entity already has a component of this type, update it
        components_[entityToIndex_[entityId]] = component;
        return;
    }

    components_.push_back(component);
    indexToEntity_.push_back(entityId);

    entityToIndex_[entityId] = size_;

    size_++;
}

// this function uses the operations of a sparse set, so we do the same O(1) as a map
// but this is a lot faster since it is basic pointer arithmetic to remove
template<typename ComponentType>
void ComponentSet<ComponentType>::remove(EntityId entityId) {
    auto it = entityToIndex_.find(entityId);

    if (it == entityToIndex_.end()) {
        // this entity doesnt have that component
        return;
    }

    size_t index_removed = it->second;

    // perform swap
    components_[index_removed] = components_[size_ - 1];
    EntityId last_entity = indexToEntity_[size_ - 1];
    indexToEntity_[index_removed] = last_entity;
    entityToIndex_[last_entity] = index_removed;
    
    // do pop
    components_.pop_back();
    entityToIndex_.erase(it);
    indexToEntity_.pop_back();

    size_--;
}

template<typename ComponentType>
ComponentType& ComponentSet<ComponentType>::get(EntityId entityId) {
    // we assume the entity exists, if not this will obviosly crash, which is good since it means
    // we wrote bad code? idk
    return components_[entityToIndex_[entityId]];
}

template<typename ComponentType>
void ComponentSet<ComponentType>::entityDestroyed(EntityId entityId) {
    remove(entityId);
}

template<typename ComponentType>
void ComponentSet<ComponentType>::clear() {
    components_.clear();
    entityToIndex_.clear();
    indexToEntity_.clear();
    size_ = 0;
}

} // namespace ecs