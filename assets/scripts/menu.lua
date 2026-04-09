local menu = {}

function menu.on_enter()
    print("Entered menu scene")
end

function menu.update(dt)
    
end

function menu.handle_input()
    if isKeyDown(Key.SPACE) then
        change_scene("demo")
    end
end

return menu