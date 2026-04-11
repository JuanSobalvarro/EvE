local intro = {}

local orbs = {}
local title_entity = -1

local bg_final_color = { r = 240, g = 240, b = 240 }

local time_since_last_spawn = 0.0
local spawn_interval = 0.1
local elapsed_time = 0.0

local function spawn_orb()
    local orb = createEntity()
    addTransform(orb, math.random(-400, 400), math.random(300, 350), 1.0, 1.0, 0.0, 1)
    addRigidBody(orb, 100, 100, false, false, 0.0, 0.0, 0.0, 0.0)
    addGeometryOrb(orb, 50, 0, 0, 255, 100)
    
    local drift_x = math.random(-100, 100)
    addVelocity(orb, drift_x, -100.0, 400.0, 500.0)
    table.insert(orbs, orb)
end

function intro.on_enter()
    print("[INTRO] Setting up scene...")
    math.randomseed(os.time())
    setWorld(1000, 1000, 100)
    
    elapsed_time = 0.0
    time_since_last_spawn = 0.0

    local bg_color = getColor(BgEntity)
    if bg_color then
        bg_color.r, bg_color.g, bg_color.b, bg_color.a = 0, 0, 0, 255
    end

    addTween(BgEntity, TweenProperty.ColorR, 0, bg_final_color.r, 5.0, 0.0, EaseType.InQuad)
    addTween(BgEntity, TweenProperty.ColorG, 0, bg_final_color.g, 5.0, 0.0, EaseType.InQuad)
    addTween(BgEntity, TweenProperty.ColorB, 0, bg_final_color.b, 5.0, 0.0, EaseType.InQuad)
    addTween(BgEntity, TweenProperty.ColorR, bg_final_color.r, 0, 5.0, 6.0, EaseType.OutQuad)
    addTween(BgEntity, TweenProperty.ColorG, bg_final_color.g, 0, 5.0, 6.0, EaseType.OutQuad)
    addTween(BgEntity, TweenProperty.ColorB, bg_final_color.b, 0, 5.0, 6.0, EaseType.OutQuad)

    title_entity = createEntity()
    addTransform(title_entity, -200.0, -200.0, 4.0, 4.0, 0.0, 2)
    addSprite(title_entity, "assets/sprites/eve.png", BlendType.Normal, 0)
    addTween(title_entity, TweenProperty.SpriteAlpha, 0, 255, 5.0, 0.0, EaseType.InQuad)
    addTween(title_entity, TweenProperty.SpriteAlpha, 255, 0, 5.0, 6.0, EaseType.OutQuad)
end

function intro.update(dt)
    elapsed_time = elapsed_time + dt

    -- Orb Spawning Logic
    time_since_last_spawn = time_since_last_spawn + dt
    if time_since_last_spawn >= spawn_interval then
        spawn_orb()
        time_since_last_spawn = 0.0
    end

    -- Orb Cleanup Logic
    for i = #orbs, 1, -1 do
        local orb = orbs[i]
        local transform = getTransform(orb)
        if transform and transform.y < -400 then
            destroyEntity(orb)
            table.remove(orbs, i)
        end
    end

    -- Trigger Scene Switch
    if elapsed_time >= 12.0 then
        change_scene("menu")
    end
end

function intro.handle_input()
    if isKeyDown(Key.SPACE) then
        change_scene("menu")
    end
end

function intro.on_exit()
    print("[INTRO] Cleaning up...")
    
    for _, orb in ipairs(orbs) do
        destroyEntity(orb)
    end
    orbs = {}

    if title_entity ~= -1 then
        destroyEntity(title_entity)
        title_entity = -1
    end

end

return intro