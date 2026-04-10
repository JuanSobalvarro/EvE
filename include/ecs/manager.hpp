#pragma once

#include <unordered_map>
#include <queue>
#include <memory>
#include <typeindex>
#include <stdexcept>
#include <algorithm>

#include "ecs/types.hpp"
#include "ecs/componentset.hpp"


namespace ecs {

class Manager {
public:
    Manager();
    ~Manager();

    std::vector<EntityId> getActiveEntities() const;

    EntityId createEntity();
    void destroyEntity(EntityId entityId);

    void clear();

    template<typename CT, typename... Args>
    void addComponent(EntityId entityId, Args&&... args) {
        getComponentSet<CT>()->insert(entityId, CT{std::forward<Args>(args)...});
    }
    
    template<typename CT>
    void removeComponent(EntityId entityId) {
        getComponentSet<CT>()->remove(entityId);
    }

    template<typename CT>
    CT& getComponent(EntityId entityId) {
        return getComponentSet<CT>()->get(entityId);
    }

    template<typename CT>
    bool hasComponent(EntityId entityId) {
        return getComponentSet<CT>()->has(entityId);
    }

    template<typename CT>
    std::vector<EntityId> getEntitiesWithComponent() {
        std::vector<EntityId> entities;
        if (componentSets_.find(typeid(CT)) == componentSets_.end()) {
            return entities; // return empty vector if no entities have this component
        }

        ComponentSet<CT>* set = getComponentSet<CT>();
        for (EntityId id : activeEntityList_) {
            if (set->has(id)) {
                entities.push_back(id);
            }
        }
        return entities;
    }

private:
    // mapping from component type to component set, we use type_index to store the type information at runtime
    // c++ assigns a unique type_index to each type, so we can use it as a key to store different component sets in the same map
    // without the manager knowing the specific type, it just needs to know the pointers to those sets
    std::unordered_map<std::type_index, std::unique_ptr<IComponentSet>> componentSets_;
    
    // queue of available entity IDs for reuse, so we dont increment infinitely and instead
    // reuse IDs of destroyed entitiess
    std::queue<EntityId> availableEntityIds_;

    // for O(1) safety check
    std::vector<bool> activeEntities_;

    // for fast access on iterations
    std::vector<EntityId> activeEntityList_;

    // get or create the component set for a specific component type
    template<typename CT>
    ComponentSet<CT>* getComponentSet() {
        std::type_index typeIndex = typeid(CT);

        if (componentSets_.find(typeIndex) == componentSets_.end()) {
            componentSets_[typeIndex] = std::make_unique<ComponentSet<CT>>();
        }

        return static_cast<ComponentSet<CT>*>(componentSets_[typeIndex].get());
    }
};

}