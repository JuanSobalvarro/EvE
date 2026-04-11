---@meta

--- This file defines the API for the game engine, which is used by Lua scripts to interact with the engine.

---@enum ShapeType
ShapeType = {
    Rectangle = 0,
    Circle = 1
}

---@enum BlendType
BlendType = {
    Normal = 0,
    Additive = 1,
    Multiply = 2
}

---@class Transform
---@field x number
---@field y number
---@field scaleX number
---@field scaleY number
---@field rotation number
---@field zIndex integer

---@class Velocity
---@field x number
---@field y number
---@field maxSpeed_x number
---@field maxSpeed_y number

---@class RigidBody
---@field width number
---@field height number
---@field isStatic boolean
---@field collidable boolean
---@field friction number
---@field gravity number
---@field mass number
---@field bounce number

---@class Sprite
---@field texture userdata
---@field blend BlendType
---@field alpha integer

---@class Color
---@field r integer
---@field g integer
---@field b integer
---@field a integer

---@class Shape
---@field type ShapeType
---@field width number
---@field height number
---@field radius number
---@field filled boolean

---@class CollisionState
---@field onGround boolean
---@field onLeftWall boolean
---@field onRightWall boolean
---@field onCeiling boolean

---@class Geometry
---@field vertices table
---@field indices table
---@field texture userdata

---@class Camera
---@field x number
---@field y number
---@field screenWidth number
---@field screenHeight number
---@field zoom number

---@enum TweenProperty
TweenProperty = {
    PositionX = 0,
    PositionY = 1,
    ScaleX = 2,
    ScaleY = 3,
    Rotation = 4,
    ColorR = 5,
    ColorG = 6,
    ColorB = 7,
    ColorA = 8,
    SpriteAlpha = 9
}

---@enum EaseType
EaseType = {
    Linear = 0,
    InQuad = 1,
    OutQuad = 2,
    InOutQuad = 3,
    InCubic = 4,
    OutCubic = 5,
    InOutCubic = 6
}

---@class TweenData
---@field property TweenProperty
---@field startValue number
---@field endValue number
---@field duration number
---@field elapsedTime number
---@field delay number
---@field easeType EaseType
---@field isActive boolean

---@class AnimationTween
---@field tweens table

---@return integer
function createEntity() end

---@param id integer
function destroyEntity(id) end

---@param id integer
---@param x number
---@param y number
---@param sx number
---@param sy number
---@param r number
---@param zIndex integer
function addTransform(id, x, y, sx, sy, r, zIndex) end

---@param id integer
---@param x number
---@param y number
---@param mx number
---@param my number
function addVelocity(id, x, y, mx, my) end

---@param id integer
---@param w number
---@param h number
---@param isStatic boolean
---@param collidable boolean
---@param friction number
---@param gravity number
---@param mass number
---@param bounce number
function addRigidBody(id, w, h, isStatic, collidable, friction, gravity, mass, bounce) end

---@param id integer
---@param texturePath string
---@param blend BlendType
---@param alpha integer
function addSprite(id, texturePath, blend, alpha) end

---@param id integer
---@param r integer
---@param g integer
---@param b integer
---@param a integer
function addColor(id, r, g, b, a) end

---@param id integer
---@param t ShapeType
---@param w number
---@param h number
---@param r number
---@param f boolean
function addShape(id, t, w, h, r, f) end

---@param id integer
function addCollisionState(id) end

---@param id integer
---@param width number
---@param height number
function addGeometryQuad(id, width, height) end

--@param id integer
--@param radius number
--@param r integer
--@param g integer
--@param b integer
--@param a integer
function addGeometryOrb(id, radius, r, g, b, a) end

---@param id integer
---@param vertices table
---@param indices table
function addGeometryCustom(id, vertices, indices) end

---@param id integer
---@param x number
---@param y number
---@param screenWidth number
---@param screenHeight number
---@param zoom number
function addCamera(id, x, y, screenWidth, screenHeight, zoom) end

---@param id integer
---@param property TweenProperty
---@param startValue number
---@param endValue number
---@param duration number
---@param delay number
---@param easeType EaseType
function addTween(id, property, startValue, endValue, duration, delay, easeType) end

---@param id integer
---@return Transform
function getTransform(id) end

---@param id integer
---@return Velocity
function getVelocity(id) end

---@param id integer
---@return RigidBody
function getRigidBody(id) end

---@param id integer
---@return Sprite
function getSprite(id) end

---@param id integer
---@return Color
function getColor(id) end

---@param id integer
---@return Shape
function getShape(id) end

---@param id integer
---@return CollisionState
function getCollisionState(id) end

---@param id integer
---@return Geometry
function getGeometry(id) end

---@param id integer
---@return AnimationTween
function getAnimationTween(id) end

---@param scancode integer
---@return boolean
function isKeyDown(scancode) end

---@param button integer
---@return boolean
function isMouseButtonDown(button) end

---@return number, number
function getMousePosition() end

function clearScene() end

---@return integer
function getEntityCount() end

---@param width integer
---@param height integer
---@param cellSize integer
function setWorld(width, height, cellSize) end

---@param id integer
---@param componentType string
function removeComponent(id, componentType) end