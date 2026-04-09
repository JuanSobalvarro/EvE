local Key = {
    A = 4,
    D = 7,
    SPACE = 44
}

local bg_entity = -1
local floor_entity = -1
local left_wall = -1
local right_wall = -1
local top_wall = -1
local ernest_entity = -1

local time_passed = 0.0

function on_enter()
    bg_entity = createEntity()
    addTransform(bg_entity, 0.0, 0.0, 1.0, 1.0, 0.0)
    addRigidBody(bg_entity, 800.0, 600.0, true, false, 1.0, 0.5)
    addShape(bg_entity, ShapeType.Rectangle, 800.0, 600.0, 0.0, true)
    addColor(bg_entity, 255, 255, 255, 255)

    floor_entity = createEntity()
    addTransform(floor_entity, 0.0, 500.0, 1.0, 1.0, 0.0)
    addRigidBody(floor_entity, 800.0, 100.0, true, true, 1.0, 0.5)
    addShape(floor_entity, ShapeType.Rectangle, 800.0, 100.0, 0.0, true)
    addColor(floor_entity, 200, 200, 200, 255)

    left_wall = createEntity()
    addTransform(left_wall, -50.0, 0.0, 1.0, 1.0, 0.0)
    addRigidBody(left_wall, 50.0, 600.0, true, true, 1.0, 0.5)
    addShape(left_wall, ShapeType.Rectangle, 50.0, 600.0, 0.0, true)

    right_wall = createEntity()
    addTransform(right_wall, 800.0, 0.0, 1.0, 1.0, 0.0)
    addRigidBody(right_wall, 50.0, 600.0, true, true, 1.0, 0.5)
    addShape(right_wall, ShapeType.Rectangle, 50.0, 600.0, 0.0, true)

    top_wall = createEntity()
    addTransform(top_wall, 0.0, -50.0, 1.0, 1.0, 0.0)
    addRigidBody(top_wall, 800.0, 50.0, true, true, 1.0, 0.5)
    addShape(top_wall, ShapeType.Rectangle, 800.0, 50.0, 0.0, true)

    ernest_entity = createEntity()
    addTransform(ernest_entity, 400.0, 450.0, 1.0, 1.0, 0.0)
    addRigidBody(ernest_entity, 50.0, 50.0, false, true, 1.0, 500.0)
    addShape(ernest_entity, ShapeType.Rectangle, 50.0, 50.0, 0.0, true)
    addColor(ernest_entity, 255, 255, 255, 255)
    addVelocity(ernest_entity, 0.0, 0.0, 400.0, 500.0)
    addCollisionState(ernest_entity)
end

function update(dt)
    time_passed = time_passed + dt
    local color = getColor(bg_entity)

    if color == nil then
        print("Error: bg_entity does not have a color component")
        return
    end

    color.r = math.floor((math.sin(time_passed) + 1) / 2 * 255)
    color.g = math.floor((math.sin(time_passed + 2) + 1) / 2 * 255)
    color.b = math.floor((math.sin(time_passed + 4) + 1) / 2 * 255)
end

function handle_input()
    local vel = getVelocity(ernest_entity)
    local col = getCollisionState(ernest_entity)

    if col == nil then
        print("Error: ernest_entity: ", ernest_entity, " does not have a collision state component")
        return
    end

    if vel == nil then
        print("Error: ernest_entity: ", ernest_entity, " does not have a velocity component")
        return
    end

    local stepSpeed = 100.0
    local jumpSpeed = 300.0

    if isKeyDown(Key.A) then
        vel.x = vel.x - stepSpeed
    end
    if isKeyDown(Key.D) then
        vel.x = vel.x + stepSpeed
    end

    local can_jump = false
    if col.onGround then
        can_jump = true
    elseif (col.onLeftWall or col.onRightWall) and vel.y >= -5 then
        can_jump = true
    end

    if isKeyDown(Key.SPACE) and can_jump then
        vel.y = vel.y - jumpSpeed
        col.onGround = false
    end
end
