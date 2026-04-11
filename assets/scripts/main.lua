package.path = package.path .. ";assets/scripts/?.lua"

Key = {
    A = 4,
    D = 7,
    SPACE = 44
}

MouseButton = {
    LEFT = 1,
    RIGHT = 2,
    MIDDLE = 3
}

BgEntity = -1
CameraEntity = -1
SceneChangerEntity = -1 

local transition_state = "IDLE" 
local transition_timer = 0.0
local transition_duration = 1.0
local pending_scene_name = ""

local active_scene = nil
local active_scene_name = "intro"

function put_scene_changer(color, duration)
    local sc_color = getColor(SceneChangerEntity)
    if sc_color and color then
        sc_color.r = color.r
        sc_color.g = color.g
        sc_color.b = color.b
        sc_color.a = 0
    end
    addTween(SceneChangerEntity, TweenProperty.ColorA, 0, 255, duration, 0.0, EaseType.InQuad)
end

function remove_scene_changer(duration)
    addTween(SceneChangerEntity, TweenProperty.ColorA, 255, 0, duration, 0.0, EaseType.OutQuad)
end

function change_scene(new_scene_name)
    if transition_state ~= "IDLE" then return end

    pending_scene_name = new_scene_name
    transition_state = "FADING_IN"
    transition_timer = transition_duration

    put_scene_changer({ r = 0, g = 0, b = 0 }, transition_duration)
end

local function perform_actual_switch()
    if active_scene and active_scene.on_exit then
        active_scene.on_exit()
    end

    if hasComponent(BgEntity, "AnimationTween") then
        removeComponent(BgEntity, "AnimationTween")
    end

    package.loaded[pending_scene_name] = nil

    active_scene_name = pending_scene_name
    active_scene = require(pending_scene_name)

    if active_scene and active_scene.on_enter then
        active_scene.on_enter()
    end
end

function on_enter()
    CameraEntity = createEntity()
    addCamera(CameraEntity, 0, 0, 800, 600, 1.0)

    BgEntity = createEntity()
    addTransform(BgEntity, -400, -300, 1.0, 1.0, 0.0, -10)
    addColor(BgEntity, 0, 0, 0, 255)
    addShape(BgEntity, ShapeType.Rectangle, 800.0, 600.0, 0.0, true)

    SceneChangerEntity = createEntity()
    addTransform(SceneChangerEntity, -400, -300, 1.0, 1.0, 0.0, 999)
    addColor(SceneChangerEntity, 0, 0, 0, 0)
    addShape(SceneChangerEntity, ShapeType.Rectangle, 800.0, 600.0, 0.0, true)

    active_scene = require(active_scene_name)
    if active_scene and active_scene.on_enter then
        active_scene.on_enter()
    end
end

function update(dt)
    if transition_state == "FADING_IN" then
        transition_timer = transition_timer - dt
        if transition_timer <= 0 then
            perform_actual_switch()
            transition_state = "FADING_OUT"
            transition_timer = transition_duration
            remove_scene_changer(transition_duration)
        end
    elseif transition_state == "FADING_OUT" then
        transition_timer = transition_timer - dt
        if transition_timer <= 0 then
            transition_state = "IDLE"
        end
    end

    if transition_state == "IDLE" then
        if active_scene and active_scene.update then
            active_scene.update(dt)
        end
    end
end

function handle_input()
    if transition_state == "IDLE" then
        if active_scene and active_scene.handle_input then
            active_scene.handle_input()
        end
    end
end