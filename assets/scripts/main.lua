
package.path = package.path .. ";assets/scripts/?.lua"

-- clear cache to allow hot-reloading of scripts
package.loaded["menu"] = nil
package.loaded["demo"] = nil

local menu = require("menu")
local demo = require("demo")
local intro = require("intro")

local current_scene = "intro"

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

function on_enter()
    if current_scene == "menu" then
        menu.on_enter()
    elseif current_scene == "demo" then
        demo.on_enter()
    elseif current_scene == "intro" then
        intro.on_enter()
    end
end

function update(dt)
    if current_scene == "menu" then
        menu.update(dt)
    elseif current_scene == "demo" then
        demo.update(dt)
    elseif current_scene == "intro" then
        intro.update(dt)
    end
end

function handle_input()
    if current_scene == "menu"  then
        menu.handle_input()
    elseif current_scene == "demo" then
        demo.handle_input()
    elseif current_scene == "intro" then
        intro.handle_input()
    end
end

function change_scene(new_scene)
    if current_scene == "menu" and menu.on_exit then
        menu.on_exit()
    elseif current_scene == "demo" and demo.on_exit then
        demo.on_exit()
    elseif current_scene == "intro" and intro.on_exit then
        intro.on_exit()
    end

    -- clear engine memory
    clearScene()

    current_scene = new_scene

    if current_scene == "menu" then
        menu.on_enter()
    elseif current_scene == "demo" then
        demo.on_enter()
    end
end