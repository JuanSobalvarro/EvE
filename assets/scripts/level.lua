local level = {}

local floorEntity = -1
local ernestEntity = -1

local jumping = false

local bullets = {}
local bullet_speed = 500.0
local bullet_cooldown_timer = 0.0
local bullet_spawn_cooldown = 0.3 -- Adjusted for better feel

local enemies = {}
local enemy_spawn_timer = 0.0
local enemy_spawn_interval = 0.5

local ulti_entity = -1
local ulti_cooldown_timer = 0.0
local ulti_spawn_cooldown = 2.0
local ulti_timer = 0.0

-- destroy enemy with alpha out effect
local function destroyEnemy(id)

    if hasComponent(id, "Sprite") then
        local color = getColor(id)
        addTween(id, TweenProperty.SpriteAlpha, 255, 0, 0.5, 0.0, EaseType.OutQuad)
        -- destroy after tween duration
        addTimer(id, 0.5, 0.0, false, function() destroyEntity(id) end)
    else
        destroyEntity(id)
    end
end

local function ulti()
    if ulti_entity ~= -1 then
        destroyEntity(ulti_entity)
    end

    ulti_entity = createEntity()
    addTransform(ulti_entity, -600, -300, 1, 1, 0.0, 2)
    addGeometryOrb(ulti_entity, 100, 106, 13, 173, 255)
    addTween(ulti_entity, TweenProperty.ScaleX, 0.0, 5.0, 6.0, 0.0, EaseType.OutQuad)
    addTween(ulti_entity, TweenProperty.ScaleY, 0.0, 5.0, 6.0, 0.0, EaseType.OutQuad)
    addTween(ulti_entity, TweenProperty.Rotation, 0.0, 360.0, 5.0, 0.0, EaseType.Linear)
    playSound("assets/sounds/ulti.wav", false, 1.0, 2.0)
    ulti_timer = 5.0

    addTimer(ulti_entity, 5.0, 0.0, false, function() 
        if hasComponent(ulti_entity, "Color") then
            local color = getColor(ulti_entity)
            addTween(ulti_entity, TweenProperty.ColorA, color.a, 0, 1.0, 0.0, EaseType.OutQuad)
        end
        addTimer(ulti_entity, 1.0, 0.0, false, function() 
            if ulti_entity ~= -1 then
                destroyEntity(ulti_entity) 
                ulti_entity = -1
            end
        end)
    end)
end

local function shoot_bullet(x, y, vx, vy)
    local bullet = createEntity()
    addTransform(bullet, x, y, 0.1, 0.1, -180, 3)
    addVelocity(bullet, vx, vy, 500.0, 500.0)
    addRigidBody(bullet, 10, 10, false, false, 0.0, 0.0, 0.0, 0.0)
    addSprite(bullet, "assets/sprites/bullet.png", BlendType.Normal, 255)
    playSound("assets/sounds/fah.wav", false, 1.0, 0.1)
    table.insert(bullets, bullet)
end

local function spawn_enemy()
    local enemy = createEntity()
    addTransform(enemy, 400, 150, 0.1, 0.1, 0.0, 2)
    addVelocity(enemy, -150.0, 0.0, 500.0, 500.0)
    addRigidBody(enemy, 40, 40, false, true, 0.0, 500.0, 0.0, 0.0)
    addSprite(enemy, "assets/sprites/alien.png", BlendType.Normal, 255)
    addCollisionState(enemy)
    table.insert(enemies, enemy)
end

function level.on_enter()
    if hasComponent(BgEntity, "Color") then
        local bg_color = getColor(BgEntity)
        bg_color.r, bg_color.g, bg_color.b, bg_color.a = 100, 100, 230, 255
    end

    floorEntity = createEntity()
    addTransform(floorEntity, -400, 200, 1.0, 1.0, 0.0, 1)
    addRigidBody(floorEntity, 800, 100, true, true, 1.0, 0.0, 0.0, 0.0)
    addShape(floorEntity, ShapeType.Rectangle, 800, 100, 0.0, true)
    addColor(floorEntity, 200, 150, 50, 255)

    ernestEntity = createEntity()
    addTransform(ernestEntity, 0, 0, 0.1, 0.1, 0.0, 2)
    addRigidBody(ernestEntity, 50, 50, false, true, 100.0, 500.0, 0.0, 0.0)
    addVelocity(ernestEntity, 0.0, 0.0, 500.0, 500.0)
    addSprite(ernestEntity, "assets/sprites/skibidi.png", BlendType.Normal, 255)
    addCollisionState(ernestEntity)
end

function level.update(dt)
    -- Enemy Spawning Logic
    enemy_spawn_timer = enemy_spawn_timer + dt
    if enemy_spawn_timer >= enemy_spawn_interval then
        spawn_enemy()
        enemy_spawn_timer = 0.0
    end

    bullet_cooldown_timer = bullet_cooldown_timer + dt
    ulti_cooldown_timer = ulti_cooldown_timer + dt

    for i = #bullets, 1, -1 do
        local b_id = bullets[i]
        local b_pos = getTransform(b_id)
        local hit = false

        -- Check if bullet hit any enemy
        for j = #enemies, 1, -1 do
            local e_id = enemies[j]
            local e_pos = getTransform(e_id)

            -- Simple Circle/Distance collision (approx 30 units)
            local dx = b_pos.x - e_pos.x
            local dy = b_pos.y - e_pos.y
            local distSq = dx*dx + dy*dy

            if distSq < 900 then
                destroyEntity(e_id)
                table.remove(enemies, j)
                hit = true
                playSound("assets/sounds/uh.wav", false, 1.0, 1.2)
                break
            end
        end

        -- Despawn if hit or out of bounds
        if hit or b_pos.x > 500 then
            destroyEntity(b_id)
            table.remove(bullets, i)
        end
    end

    for i = #enemies, 1, -1 do
        local e_id = enemies[i]
        local e_pos = getTransform(e_id)

        -- despawn logic
        if e_pos.x < -500 then
            destroyEntity(e_id)
            table.remove(enemies, i)
        end

        -- randomly jump
        if math.random() < 0.01 and hasComponent(e_id, "CollisionState") then
            local collision_state = getCollisionState(e_id)
            if collision_state.onGround then
                local e_vel = getVelocity(e_id)
                e_vel.y = -400.0
            end
        end

        -- move to the left 
        local e_vel = getVelocity(e_id)
        e_vel.x = -150.0
    end


    -- check ulti collision, if enemies destroy entities
    if ulti_timer > 0 then
        ulti_timer = ulti_timer - dt
        if ulti_timer <= 0 then
            local ulti_pos = getTransform(ulti_entity)
            for i, e_id in ipairs(enemies) do
                if hasComponent(e_id, "Transform") then
                    local e_pos = getTransform(e_id)
                    local dx = ulti_pos.x - e_pos.x
                    local dy = ulti_pos.y - e_pos.y
                    local distSq = dx*dx + dy*dy

                    if distSq < 40000 then -- 200 units radius
                        destroyEnemy(e_id)
                        -- playSound("assets/sounds/uh.wav", false, 1.0, 1.2)
                    end
                end
            end
            enemies = {}
        end
    end
end

function level.handle_input()
    if not hasComponent(ernestEntity, "Velocity") then return end
    local velocity = getVelocity(ernestEntity)

    if isKeyDown(Key.A) then velocity.x = velocity.x - 200.0 end
    if isKeyDown(Key.D) then velocity.x = velocity.x + 200.0 end

    if hasComponent(ernestEntity, "CollisionState") then
        local collision_state = getCollisionState(ernestEntity)
        if collision_state.onGround then
            jumping = false
            if isKeyDown(Key.W) then
                velocity.y = velocity.y - 600.0
                jumping = true
            end
        end
    end

    if isKeyDown(Key.SPACE) and bullet_cooldown_timer >= bullet_spawn_cooldown then
        local pos = getTransform(ernestEntity)
        shoot_bullet(pos.x + 20, pos.y, bullet_speed, 0.0)
        bullet_cooldown_timer = 0.0
    end

    -- ULTI spawn big orb
    if isKeyDown(Key.R) and ulti_cooldown_timer >= ulti_spawn_cooldown then
        print("ULTI!")
        ulti()
        ulti_cooldown_timer = 0.0
    end
end

function level.on_exit()
    for _, b in ipairs(bullets) do destroyEntity(b) end
    for _, e in ipairs(enemies) do destroyEntity(e) end
    if ernestEntity ~= -1 then destroyEntity(ernestEntity) end
    if floorEntity ~= -1 then destroyEntity(floorEntity) end
    bullets = {}
    enemies = {}
end

return level