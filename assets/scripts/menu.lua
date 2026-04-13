local menu = {}

local title_entity = -1
local subtitle_entity = -1
local play_button = -1
local exit_button = -1
local play_glow = -1
local exit_glow = -1
local decor_orbs = {}

local mouse_was_down = false
local elapsed_time = 0.0
local active_button = "none"

local title_intro_tween_started = false

local BUTTONS = {
    play = { x = -160, y = 52, w = 320, h = 68 },
    exit = { x = -160, y = 138, w = 320, h = 68 }
}

local function rect_contains(rect, px, py)
    return px >= rect.x and px <= (rect.x + rect.w) and py >= rect.y and py <= (rect.y + rect.h)
end

local function create_glow(x, y, radius, r, g, b, alpha, z_index)
    local orb = createEntity()
    addTransform(orb, x, y, 1.0, 1.0, 0.0, z_index)
    addGeometryOrb(orb, radius, r, g, b, alpha)
    return orb
end

local function setup_button(button_entity, glow_entity, rect, base_r, base_g, base_b)
    addTransform(button_entity, rect.x, rect.y, 1.0, 1.0, 0.0, 20)
    addColor(button_entity, base_r, base_g, base_b, 230)
    addShape(button_entity, ShapeType.Rectangle, rect.w, rect.h, 0.0, true)

    addTransform(glow_entity, rect.x - 22, rect.y - 12, 1.0, 1.0, 0.0, 18)
    addGeometryOrb(glow_entity, 120, base_r, base_g, base_b, 100)

    addTween(glow_entity, TweenProperty.ScaleX, 1.0, 1.08, 2.4, 0.0, EaseType.InOutQuad)
    addTween(glow_entity, TweenProperty.ScaleY, 1.0, 1.08, 2.4, 0.0, EaseType.InOutQuad)
    addTween(glow_entity, TweenProperty.Rotation, 0.0, 360.0, 20.0, 0.0, EaseType.Linear)
end

local function spawn_decor()
    decor_orbs = {}

    local orb_specs = {
        { x = -300, y = -130, radius = 58, r = 70, g = 90, b = 255, alpha = 85, dx = 30, dy = 18 },
        { x = 270, y = -180, radius = 72, r = 255, g = 120, b = 80, alpha = 75, dx = -35, dy = 22 },
        { x = -40, y = 240, radius = 96, r = 120, g = 255, b = 190, alpha = 55, dx = 25, dy = -16 }
    }

    for i, spec in ipairs(orb_specs) do
        local orb = create_glow(spec.x, spec.y, spec.radius, spec.r, spec.g, spec.b, spec.alpha, -6 + i)
        addTween(orb, TweenProperty.PositionX, spec.x, spec.x + spec.dx, 5.0 + i, 0.0, EaseType.InOutQuad)
        addTween(orb, TweenProperty.PositionY, spec.y, spec.y + spec.dy, 4.2 + i, 0.0, EaseType.InOutQuad)
        addTween(orb, TweenProperty.ScaleX, 1.0, 1.25, 3.2 + i * 0.4, 0.0, EaseType.InOutQuad)
        addTween(orb, TweenProperty.ScaleY, 1.0, 1.25, 3.2 + i * 0.4, 0.0, EaseType.InOutQuad)
        table.insert(decor_orbs, orb)
    end
end

local function update_button_visuals(button_entity, rect, hovered, base_r, base_g, base_b)
    local transform = getTransform(button_entity)
    local color = getColor(button_entity)

    if transform and color then
        local target_scale = hovered and 1.05 or 1.0
        local pulse = hovered and (math.sin(elapsed_time * 10.0) * 0.02) or 0.0
        transform.scaleX = target_scale + pulse
        transform.scaleY = target_scale + pulse
        transform.x = rect.x - ((transform.scaleX - 1.0) * rect.w * 0.5)
        transform.y = rect.y - ((transform.scaleY - 1.0) * rect.h * 0.5)

        color.r = hovered and math.min(255, base_r + 25) or base_r
        color.g = hovered and math.min(255, base_g + 25) or base_g
        color.b = hovered and math.min(255, base_b + 25) or base_b
        color.a = hovered and 245 or 230
    end
end

function menu.on_enter()
    print("[MENU] Entered menu scene")

    elapsed_time = 0.0
    mouse_was_down = false
    active_button = "none"

    addTween(BgEntity, TweenProperty.ColorR, 0, 18, 0.8, 0.0, EaseType.InQuad)
    addTween(BgEntity, TweenProperty.ColorG, 0, 20, 0.8, 0.0, EaseType.InQuad)
    addTween(BgEntity, TweenProperty.ColorB, 0, 34, 0.8, 0.0, EaseType.InQuad)
    addTween(BgEntity, TweenProperty.ColorA, 0, 255, 0.8, 0.0, EaseType.InQuad)

    spawn_decor()

    play_button = createEntity()
    play_glow = createEntity()
    setup_button(play_button, play_glow, BUTTONS.play, 58, 120, 255)
    
    exit_button = createEntity()
    exit_glow = createEntity()
    setup_button(exit_button, exit_glow, BUTTONS.exit, 255, 88, 88)

    title_entity = createEntity()
    addTransform(title_entity, -150.0, -1000.0, 0.05, 0.05, -360.0, 25)
    addSprite(title_entity, "assets/sprites/eve.png", BlendType.Normal, 255)
    addTween(title_entity, TweenProperty.PositionY, -1000.0, -235.0, 5.0, 0.0, EaseType.OutCubic)
    addTween(title_entity, TweenProperty.PositionX, -1000.0, -210.0, 5.0, 0.0, EaseType.OutCubic)
    addTween(title_entity, TweenProperty.ScaleX, 0.05, 0.2, 5.0, 0.0, EaseType.OutCubic)
    addTween(title_entity, TweenProperty.ScaleY, 0.05, 0.2, 5.0, 0.0, EaseType.OutCubic)
    addTween(title_entity, TweenProperty.Rotation, -360.0, 0.0, 5.0, 0.0, EaseType.InOutQuad)
    addTween(title_entity, TweenProperty.PositionY, -235.0, -300.0, 5.0, 5.0, EaseType.InOutQuad)

    subtitle_entity = createEntity()
    addTransform(subtitle_entity, -170.0, 0.0, 1.0, 1.0, 0.0, 24)
    addColor(subtitle_entity, 180, 210, 255, 190)
    addShape(subtitle_entity, ShapeType.Rectangle, 340.0, 6.0, 0.0, true)

    playSound("assets/sounds/efn.wav", true, 1.0, 1.0)
end

function menu.update(dt)
    elapsed_time = elapsed_time + dt

    local mouse_x, mouse_y = getMousePosition()
    local world_x = mouse_x - 400.0
    local world_y = mouse_y - 300.0

    local play_hovered = rect_contains(BUTTONS.play, world_x, world_y)
    local exit_hovered = rect_contains(BUTTONS.exit, world_x, world_y)

    active_button = "none"
    if play_hovered then
        active_button = "play"
    elseif exit_hovered then
        active_button = "exit"
    end

    update_button_visuals(play_button, BUTTONS.play, play_hovered, 58, 120, 255)
    update_button_visuals(exit_button, BUTTONS.exit, exit_hovered, 255, 88, 88)

    local title_transform = getTransform(title_entity)
    if title_transform then
        title_transform.rotation = math.sin(elapsed_time * 1.4) * 4.0
    end

    for i, orb in ipairs(decor_orbs) do
        local transform = getTransform(orb)
        if transform then
            transform.rotation = transform.rotation + dt * (2.0 + i)
        end
    end
end

function menu.handle_input()
    local mouse_down = isMouseButtonDown(MouseButton.LEFT)
    local just_pressed = mouse_down and not mouse_was_down

    if just_pressed then
        if active_button == "play" then
            stopAllSounds()
            change_scene("demo")
        elseif active_button == "exit" then
            quitGame()
        end
    end

    mouse_was_down = mouse_down
end

function menu.on_exit()
    print("[MENU] Leaving menu scene")

    if title_entity ~= -1 then
        destroyEntity(title_entity)
        title_entity = -1
    end

    if subtitle_entity ~= -1 then
        destroyEntity(subtitle_entity)
        subtitle_entity = -1
    end

    if play_button ~= -1 then
        destroyEntity(play_button)
        play_button = -1
    end

    if exit_button ~= -1 then
        destroyEntity(exit_button)
        exit_button = -1
    end

    if play_glow ~= -1 then
        destroyEntity(play_glow)
        play_glow = -1
    end

    if exit_glow ~= -1 then
        destroyEntity(exit_glow)
        exit_glow = -1
    end

    for _, orb in ipairs(decor_orbs) do
        destroyEntity(orb)
    end
    decor_orbs = {}

    stopAllSounds()
end

return menu