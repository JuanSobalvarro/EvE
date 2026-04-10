
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

local active_scene = nil --- This will hold the currently active scene module
local active_scene_name = "intro" --- this holds the name of the active_scene which should be the same as the file name

--- global entities
BgEntity = -1
CameraEntity = -1


function change_scene(new_scene_name)
    if active_scene and active_scene.on_exit then
        active_scene.on_exit()
        --- clearScene() --- IGNORE --- why ignore? because we could have global entities
    end

    -- clear lua cache
    package.loaded[new_scene_name] = nil

    -- dynamically load the new scene module
    active_scene_name = new_scene_name
    active_scene = require(new_scene_name)

    if active_scene and active_scene.on_enter then
        active_scene.on_enter()
    end
end

function on_enter()
    CameraEntity = createEntity()
    addCamera(CameraEntity, 0, 0, 800, 600, 1.0)

    BgEntity = createEntity()
    addTransform(BgEntity, -400, -300, 1.0, 1.0, 0.0)
    addColor(BgEntity, 0, 0, 0, 255)
    addShape(BgEntity, ShapeType.Rectangle, 800.0, 600.0, 0.0, true)

    change_scene(active_scene_name)
end

function update(dt)
    -- If the active scene has an update function, run it
    if active_scene and active_scene.update then
        active_scene.update(dt)
    end
end

function handle_input()
    -- If the active scene has a handle_input function, run it
    if active_scene and active_scene.handle_input then
        active_scene.handle_input()
    end
end