local intro = {}

local bg_entity = -1
local left_wall = -1
local right_wall = -1
local top_wall = -1
local floor = -1

function intro.on_enter()
    print("Entered intro scene")
end

function intro.update(dt)
end

function intro.handle_input()
    if isKeyDown(Key.SPACE) then
        change_scene("menu")
    end
end


function intro.on_exit()
end

return intro