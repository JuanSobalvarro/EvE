local intro = {}

local balls = {}
local init_balls = 1000
local bg_entity = -1
local left_wall = -1
local right_wall = -1
local top_wall = -1
local floor = -1

local time_since_last_spawn = 0.0
local spawn_interval = 0.1

local function add_random_ball(x, y)
    local ball = createEntity()
    addTransform(ball, x, y, 1.0, 1.0, 0.0)
    addRigidBody(ball, 25.0, 25.0, false, false, 1.0, 0.0, 1.0, 1.0)
    addShape(ball, ShapeType.Rectangle, 25.0, 25.0, 0.0, true)
    addColor(ball, math.random(255), math.random(255), math.random(255), 255)
    addVelocity(ball, math.random(-200, 200), math.random(-200, 200), 400.0, 500.0)
    addCollisionState(ball)
    table.insert(balls, #balls + 1, ball)
end

function intro.on_enter()
    print("Entered intro scene")
    setWorld(1000, 1000, 100)

    bg_entity = createEntity()
    addTransform(bg_entity, 0.0, 0.0, 1.0, 1.0, 0.0)
    addRigidBody(bg_entity, 800.0, 600.0, true, false, 1.0, 0.0)
    addShape(bg_entity, ShapeType.Rectangle, 800.0, 600.0, 0.0, true)
    addColor(bg_entity, 255, 255, 255, 255, 255)

    left_wall = createEntity()
    addTransform(left_wall, -50.0, 0.0, 1.0, 1.0, 0.0)
    addRigidBody(left_wall, 50.0, 600.0, true, true, 0.0, 0.0)
    addShape(left_wall, ShapeType.Rectangle, 50.0, 600.0, 0.0, true)
    
    right_wall = createEntity()
    addTransform(right_wall, 800.0, 0.0, 1.0, 1.0, 0.0)
    addRigidBody(right_wall, 50.0, 600.0, true, true, 0.0, 0.0)
    addShape(right_wall, ShapeType.Rectangle, 50.0, 600.0, 0.0, true)

    top_wall = createEntity()
    addTransform(top_wall, 0.0, -50.0, 1.0, 1.0, 0.0)
    addRigidBody(top_wall, 800.0, 50.0, true, true, 0.0, 0.0)
    addColor(top_wall, 100, 100, 100, 255)
    addShape(top_wall, ShapeType.Rectangle, 800.0, 50.0, 0.0, true)

    floor = createEntity()
    addTransform(floor, 0.0, 600.0, 1.0, 1.0, 0.0)
    addRigidBody(floor, 800.0, 50.0, true, true, 0.0, 0.0, 0.0, 0.0)
    addColor(floor, 100, 100, 100, 255)
    addShape(floor, ShapeType.Rectangle, 800.0, 50.0, 0.0, true)

    for i=1,init_balls do
        add_random_ball(math.random(0, 800), math.random(0, 600))
    end
end

function intro.update(dt)
    time_since_last_spawn = time_since_last_spawn + dt
    -- for _, ball in ipairs(balls) do
    --     l
    -- end
end

function intro.handle_input()
    if isMouseButtonDown(MouseButton.LEFT) and time_since_last_spawn >= spawn_interval then
        local x, y = getMousePosition()
        add_random_ball(x, y)
        print("Current entities: ", getEntityCount())
        time_since_last_spawn = 0.0
    end

    if isKeyDown(Key.SPACE) then
        for _, ball in ipairs(balls) do
            destroyEntity(ball)
        end
    end
end


function intro.on_exit()
end

return intro