#include "ecs/animation.hpp"

void ecs::AnimationSystem::update(ecs::Manager& manager, float deltaTime) {
    std::vector<EntityId> entities = manager.getEntitiesWithComponent<AnimationTween>();

    for (EntityId id : entities) {
        AnimationTween& anim = manager.getComponent<AnimationTween>(id);

        // std::cout << "Updating entity " << id << " with " << anim.tweens.size() << " active tweens" << std::endl;

        for (auto& tween : anim.tweens) {
            if (!tween.active) continue;

            tween.elapsedTime += deltaTime;
            
            // Only update if we are past the delay
            if (tween.elapsedTime >= tween.delay) {
                updateTween(manager, id, tween);

                if (tween.elapsedTime >= (tween.delay + tween.duration)) {
                    tween.active = false;
                }
            }
        }

        // Clean up dead tweens
        auto newEnd = std::remove_if(anim.tweens.begin(), anim.tweens.end(), 
            [](const AnimationTween::TweenData& t) { return !t.active; });
        
        if (newEnd != anim.tweens.end()) {
            // Optional: log which ones died here if you need to debug
            anim.tweens.erase(newEnd, anim.tweens.end());
        }
    }
}

void ecs::AnimationSystem::updateTween(ecs::Manager& manager, EntityId entityId, AnimationTween::TweenData& tween) {
    
    if (tween.duration <= 0.0f) {
        tween.active = false;
        return;
    }
    
    float t = (tween.elapsedTime - tween.delay) / tween.duration;
    t = std::clamp(t, 0.0f, 1.0f);

    // apply easing
    switch (tween.easeType) {
        case EaseType::Linear:
            break;
        case EaseType::InQuad:
            t = t * t;
            break;
        case EaseType::OutQuad:
            t = t * (2 - t);
            break;
        case EaseType::InOutQuad:
            t = (t < 0.5f) ? (2 * t * t) : (-1 + (4 - 2 * t) * t);
            break;
        case EaseType::InCubic:
            t = t * t * t;
            break;
        case EaseType::OutCubic: {
            float f = t - 1.0f;
            t = f * f * f + 1.0f;
            break;
        }
        case EaseType::InOutCubic:
            t = (t < 0.5f) ? (4 * t * t * t) : ((t - 1) * (2 * t - 2) * (2 * t - 2) + 1);
            break;
        default:
            throw std::runtime_error("Unsupported ease type");
    }

    float newValue = tween.startValue + (tween.endValue - tween.startValue) * t;

    // apply the new value to the appropriate component based on the property type
    switch (tween.property) {
        case TweenProperty::PositionX:
            if (manager.hasComponent<Transform>(entityId)) {
                manager.getComponent<Transform>(entityId).x = newValue;
            }
            break;
        case TweenProperty::PositionY:
            if (manager.hasComponent<Transform>(entityId)) {
                manager.getComponent<Transform>(entityId).y = newValue;
            }
            break;
        case TweenProperty::ScaleX:
            if (manager.hasComponent<Transform>(entityId)) {
                manager.getComponent<Transform>(entityId).scaleX = newValue;
            }
            break;
        case TweenProperty::ScaleY:
            if (manager.hasComponent<Transform>(entityId)) {
                manager.getComponent<Transform>(entityId).scaleY = newValue;
            }
            break;
        case TweenProperty::Rotation:
            if (manager.hasComponent<Transform>(entityId)) {
                manager.getComponent<Transform>(entityId).rotation = newValue;
            }
            break;
        case TweenProperty::ColorR:
            if (manager.hasComponent<Color>(entityId)) {
                manager.getComponent<Color>(entityId).r = static_cast<uint8_t>(newValue);
            }
            break;
        case TweenProperty::ColorG:
            if (manager.hasComponent<Color>(entityId)) {
                manager.getComponent<Color>(entityId).g = static_cast<uint8_t>(newValue);
            }
            break;
        case TweenProperty::ColorB:
            if (manager.hasComponent<Color>(entityId)) {
                manager.getComponent<Color>(entityId).b = static_cast<uint8_t>(newValue);
            }
            break;
        case TweenProperty::ColorA:
            if (manager.hasComponent<Color>(entityId)) {
                manager.getComponent<Color>(entityId).a = static_cast<uint8_t>(newValue);
            }
            break;
        case TweenProperty::SpriteAlpha:
            if (manager.hasComponent<Sprite>(entityId)) {
                manager.getComponent<Sprite>(entityId).alpha = static_cast<uint8_t>(newValue);
            }
            break;
        default:
            throw std::runtime_error("Unsupported tween property");
    }
}