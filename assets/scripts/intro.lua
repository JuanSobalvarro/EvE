local intro = {}

local balls = {}
local bg_entity = -1
local left_wall = -1
local right_wall = -1
local top_wall = -1
local floor = -1

function intro.on_enter()
    print("Entered intro scene")
    setWorldSize(1000, 1000)

    bg_entity = createEntity()
    addTransform(bg_entity, 0.0, 0.0, 1.0, 1.0, 0.0)
    addRigidBody(bg_entity, 800.0, 600.0, true, false, 1.0, 0.5)
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

    for i=1,10 do
        local ball = createEntity()
        addTransform(ball, math.random(0, 800), math.random(0, 600), 1.0, 1.0, 0.0)
        addRigidBody(ball, 25.0, 25.0, false, true, 1.0, 500.0)
        addShape(ball, ShapeType.Rectangle, 25.0, 25.0, 0.0, true)
        addColor(ball, math.random(255), math.random(255), math.random(255), 255)
        addVelocity(ball, math.random(-200, 200), math.random(-200, 200), 400.0, 500.0)
        addCollisionState(ball)
        balls[i] = ball
    end
end

function intro.update(dt)
    -- for _, ball in ipairs(balls) do
    --     l
    -- end
end

function intro.handle_input()
    if isMouseButtonDown(MouseButton.LEFT) then
        local x, y = getMousePosition()
        local ball = createEntity()
        addTransform(ball, x, y, 1.0, 1.0, 0.0)
        addRigidBody(ball, 25.0, 25.0, false, true, 1.0, 500.0)
        addShape(ball, ShapeType.Rectangle, 25.0, 25.0, 0.0, true)
        addColor(ball, math.random(255), math.random(255), math.random(255), 255)
        addVelocity(ball, math.random(-200, 200), math.random(-200, 200), 400.0, 500.0)
        addCollisionState(ball)
        table.insert(balls, #balls + 1, ball)
        print("Current entities: ", getEntityCount())
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