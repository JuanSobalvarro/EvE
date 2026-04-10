local intro = {}

local orbs = {}
local title_entity = -1

local bg_initial_color = { r = 0, g = 0, b = 0 }
local bg_final_color = { r = 100, g = 240, b = 240 }

local time_since_last_spawn = 0.0
local spawn_interval = 0.1

local elapsed_time = 0.0

local function spawn_orb()
    local orb = createEntity()
    addTransform(orb, math.random(-400, 400), math.random(300, 350), 1.0, 1.0, 0.0)
    addRigidBody(orb, 100, 100, false, false, 0.0, 0.0, 0.0, 0.0)
    addGeometryOrb(orb, 50, 0, 0, 255, 100)
    
    local drift_x = math.random(-100, 100)
    addVelocity(orb, drift_x, -100.0, 400.0, 500.0)
    table.insert(orbs, orb)
end

function intro.on_enter()
    print("Entered intro scene")
    math.randomseed(os.time())
    setWorld(1000, 1000, 100)

    title_entity = createEntity()
    addTransform(title_entity, -200.0, -200.0, 4.0, 4.0, 0.0)
    addSprite(title_entity, "assets/sprites/eve.png", BlendType.Normal, 0)
    
    local sprite = getSprite(title_entity)
    if sprite then sprite.alpha = 0 end
end

function intro.update(dt)
    elapsed_time = elapsed_time + dt

    time_since_last_spawn = time_since_last_spawn + dt
    if time_since_last_spawn >= spawn_interval then
        spawn_orb()
        time_since_last_spawn = 0.0
    end

    for i = #orbs, 1, -1 do
        local orb = orbs[i]
        local transform = getTransform(orb)
        if transform and transform.y < -400 then
            destroyEntity(orb)
            table.remove(orbs, i)
        end
    end
end

function intro.handle_input()
    if isKeyDown(Key.SPACE) then
        change_scene("menu")
    end
end

function intro.on_exit()
    -- fade out orbs and bg_entity
    for _, orb in ipairs(orbs) do
        local color = getColor(orb)
        if color then
            color.a = 0
        end
    end
    local bg_color = getColor(BgEntity)
    if bg_color then
        bg_color.a = 0
    end

    -- destroy entities
    for _, orb in ipairs(orbs) do
        destroyEntity(orb)
    end

    if title_entity ~= -1 then
        destroyEntity(title_entity)
        title_entity = -1
    end

    -- print("Current entities: ", getEntityCount())
end

return intro