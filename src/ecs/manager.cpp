#include "ecs/manager.hpp"

ecs::Manager::Manager() {
    // initialize the queue of available entity IDs, remembering that id 0 is reserved
    // for invalid entities
    activeEntities_.resize(MAX_ENTITIES, false);
    for (EntityId id = 1; id < MAX_ENTITIES; ++id) {
        availableEntityIds_.push(id);
    }
}

ecs::Manager::~Manager() {
    // nothing to clean up since we are using smart pointers for component sets
}

ecs::EntityId ecs::Manager::createEntity() {
    if (availableEntityIds_.empty()) {
        throw std::runtime_error("Maximum number of entities reached");
    }

    EntityId newEntityId = availableEntityIds_.front();
    availableEntityIds_.pop();

    activeEntities_[newEntityId] = true;
    activeEntityList_.push_back(newEntityId);

    return newEntityId;
}

void ecs::Manager::destroyEntity(EntityId entityId) {
    if (entityId >= activeEntities_.size() || !activeEntities_[entityId]) {
        return;
    }

    // return the entity ID to the queue for reuse
    availableEntityIds_.push(entityId);
    activeEntities_[entityId] = false;

    // remove the entity from the active entity list
    auto it = std::find(activeEntityList_.begin(), activeEntityList_.end(), entityId);
    if (it != activeEntityList_.end()) {
        activeEntityList_.erase(it);
    }

    // notify all component sets that this entity has been destroyed, so they can remove any components associated with it
    for (auto& pair : componentSets_) {
        pair.second->entityDestroyed(entityId);
    }

}

std::vector<ecs::EntityId> ecs::Manager::getActiveEntities() const {
    return activeEntityList_;
}


