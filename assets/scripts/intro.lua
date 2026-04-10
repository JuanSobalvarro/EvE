local intro = {}

local orbs = {}
local camera = -1
local bg_entity = -1
local title_entity = -1

local bg_current_color = { r = 0, g = 0, b = 0 }
local bg_final_color = { r = 240, g = 240, b = 240 }

local elapsed_time = 0.0
local fade_in_duration = 2.0
local hold_duration = 3
local fade_out_duration = 2.0

local time_since_last_spawn = 0.0
local spawn_interval = 0.1

local function smoothstep(edge0, edge1, x)
    x = math.max(0, math.min(1, (x - edge0) / (edge1 - edge0)))
    return x * x * (3 - 2 * x)
end

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

    camera = createEntity()
    addCamera(camera, 0, 0, 800, 600, 1.0)

    bg_entity = createEntity()
    addTransform(bg_entity, -400.0, -300.0, 1.0, 1.0, 0.0)
    addRigidBody(bg_entity, 800.0, 600.0, true, false, 1.0, 0.0, 0.0, 0.0)
    addShape(bg_entity, ShapeType.Rectangle, 800.0, 600.0, 0.0, true)
    addColor(bg_entity, 0, 0, 0, 255)

    title_entity = createEntity()
    addTransform(title_entity, -200.0, -200.0, 4.0, 4.0, 0.0)
    addSprite(title_entity, "assets/sprites/eve.png", BlendType.Normal, 0)
    
    local sprite = getSprite(title_entity)
    if sprite then sprite.alpha = 0 end
end

function intro.update(dt)
    if bg_entity == -1 then return end
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

    local bg_speed = 100.0 * dt
    local function move_towards(curr, target, step)
        if curr < target then return math.min(curr + step, target) end
        if curr > target then return math.max(curr - step, target) end
        return curr
    end

    bg_current_color.r = move_towards(bg_current_color.r, bg_final_color.r, bg_speed)
    bg_current_color.g = move_towards(bg_current_color.g, bg_final_color.g, bg_speed)
    bg_current_color.b = move_towards(bg_current_color.b, bg_final_color.b, bg_speed)

    local color_comp = getColor(bg_entity)
    if color_comp then
        color_comp.r, color_comp.g, color_comp.b = math.floor(bg_current_color.r), math.floor(bg_current_color.g), math.floor(bg_current_color.b)
    end

    if title_entity ~= -1 then
        local sprite = getSprite(title_entity)
        if sprite then
            local alpha_factor = 0.0

            if elapsed_time < fade_in_duration then
                -- fade in
                alpha_factor = smoothstep(0, fade_in_duration, elapsed_time)
            elseif elapsed_time < (fade_in_duration + hold_duration) then
                -- hold
                alpha_factor = 1.0
            elseif elapsed_time < (fade_in_duration + hold_duration + fade_out_duration) then
                -- fade out
                local fade_out_start = fade_in_duration + hold_duration
                alpha_factor = 1.0 - smoothstep(fade_out_start, fade_out_start + fade_out_duration, elapsed_time)
            else
                --- if we reached 0.0 then change to menu
                change_scene("menu")
                alpha_factor = 0.0
            end

            sprite.alpha = math.floor(alpha_factor * 255)
        end
    end
end

function intro.handle_input()
    if isKeyDown(Key.SPACE) then
        change_scene("menu")
    end
end

function intro.on_exit()
    --- fade out orbs and bg_entity
    for _, orb in ipairs(orbs) do
        local color = getColor(orb)
        if color then
            color.a = 0
        end
    end
    local bg_color = getColor(bg_entity)
    if bg_color then
        bg_color.a = 0
    end

    clearScene()
end

return intro