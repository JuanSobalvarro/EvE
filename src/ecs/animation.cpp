#include "ecs/animation.hpp"

void ecs::AnimationSystem::update(ecs::Manager& manager, float deltaTime) {
    std::vector<EntityId> entities = manager.getEntitiesWithComponent<Animation>();

    for (EntityId id : entities) {
        Animation& anim = manager.getComponent<Animation>(id);
        for (Animation::TweenData& tween : anim.tweens) {
            if (!tween.active) continue;

            tween.elapsedTime += deltaTime;
            if (tween.elapsedTime < tween.delay) continue;

            updateTween(manager, id, tween);

            if (tween.elapsedTime >= tween.delay + tween.duration) {
                tween.active = false; // mark as inactive when done
            }
        }
    }
}

void ecs::AnimationSystem::updateTween(ecs::Manager& manager, EntityId entityId, Animation::TweenData& tween) {
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