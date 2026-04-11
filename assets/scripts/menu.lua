local menu = {}

local title_entity = -1

function menu.on_enter()
    print("Entered menu scene")
    
    addTween(BgEntity, TweenProperty.ColorR, 0, 100, 0.5, 0.0, EaseType.InQuad)
    addTween(BgEntity, TweenProperty.ColorG, 0, 100, 0.5, 0.0, EaseType.InQuad)
    addTween(BgEntity, TweenProperty.ColorB, 0, 100, 0.5, 0.0, EaseType.InQuad)
    addTween(BgEntity, TweenProperty.ColorA, 0, 255, 0.5, 0.0, EaseType.InQuad)
    
    title_entity = createEntity()
    addTransform(title_entity, -200.0, -400.0, 4.0, 4.0, 0.0, 2)
    addSprite(title_entity, "assets/sprites/eve.png", BlendType.Normal, 0)
end

function menu.update(dt)
    
end

function menu.handle_input()
    -- if isKeyDown(Key.SPACE) then
    --     change_scene("demo")
    -- end
end

return menu