local intro = {}

local orbs = {}
local camera = -1
local bg_entity = -1

local bg_current_color = { r = 0, g = 0, b = 0 }
local bg_final_color = { r = 240, g = 240, b = 240 }

local title_entity = -1

local time_since_last_spawn = 0.0
local spawn_interval = 0.1

local function spawn_orb()
    local orb = createEntity()
    
    addTransform(orb, math.random(-400, 400), math.random(300, 350), 1.0, 1.0, 0.0)
    addRigidBody(orb, 100, 100, false, false, 0.0, 0.0, 0.0, 0.0)
    addGeometryOrb(orb, 50, 0, 0, 255, 255)
    
    local drift_x = math.random(-100, 100)
    addVelocity(orb, drift_x, -100.0, 400.0, 500.0)
    
    table.insert(orbs, orb)
end

function intro.on_enter()
    print("Entered intro scene")
    
    math.randomseed(os.time())
    
    setWorld(1000, 1000, 100)

    camera = createEntity()
    addCamera(camera, 0, 0, 800, 600, 1.0)

    bg_entity = createEntity()
    -- Shift the background so its center aligns with 0,0
    addTransform(bg_entity, -400.0, -300.0, 1.0, 1.0, 0.0)
    addRigidBody(bg_entity, 800.0, 600.0, true, false, 1.0, 0.0, 0.0, 0.0)
    addShape(bg_entity, ShapeType.Rectangle, 800.0, 600.0, 0.0, true)
    
    -- Set initial color
    addColor(bg_entity, bg_current_color.r, bg_current_color.g, bg_current_color.b, 255)

    for i = 1, 10 do
        spawn_orb()
    end

    -- title sprite
    title_entity = createEntity()
    addTransform(title_entity, -200.0, -200.0, 4.0, 4.0, 0.0)
    addSprite(title_entity, "assets/sprites/eve.png", BlendType.Normal)
end

function intro.update(dt)
    if bg_entity == -1 then return end

    time_since_last_spawn = time_since_last_spawn + dt
    if time_since_last_spawn >= spawn_interval then
        spawn_orb()
        time_since_last_spawn = 0.0
    end

    for i = #orbs, 1, -1 do
        local orb = orbs[i]
        local transform = getTransform(orb)
        
        -- The top of the screen is now -300, so destroy them at -300 - 100 (to account for their radius)
        if transform and transform.y < -400 then
            destroyEntity(orb)
            table.remove(orbs, i)
        end
    end

    local speed = 100.0 * dt
    
    -- Proper move_towards helper so it can fade both up AND down
    local function move_towards(current, target, step)
        if current < target then
            return math.min(current + step, target)
        elseif current > target then
            return math.max(current - step, target)
        end
        return current
    end

    bg_current_color.r = move_towards(bg_current_color.r, bg_final_color.r, speed)
    bg_current_color.g = move_towards(bg_current_color.g, bg_final_color.g, speed)
    bg_current_color.b = move_towards(bg_current_color.b, bg_final_color.b, speed)

    local color = getColor(bg_entity)

    if color == nil then
        return
    end
    
    color.r = math.floor(bg_current_color.r)
    color.g = math.floor(bg_current_color.g)
    color.b = math.floor(bg_current_color.b)
end

function intro.handle_input()
    if isKeyDown(Key.SPACE) then
        change_scene("menu")
    end
end

function intro.on_exit()
end

return intro