local Look = require("lib._look")

---@class Player
Player = {}

---@param player_id number
function Player:new(player_id)
    self.__index = self
    self.player_id = player_id
    return self
end

---@return string | nil
function Player:get_username()
    return __Player:get_username(self.player_id)
end

---@param username string
---@return nil
function Player:set_username(username)
    return __Player:set_username(self.player_id, username)
end

---@return Client
function Player:get_client()
    local client = Client:new(self.player_id)
    return client
end

---@return Position
function Player:get_position()
    local t = __Player:get_position(self.player_id)
    local pos = Position:new(t.x, t.y, t.z)
    return pos
end

---@param position Position
---@return nil
function Player:set_position(position)
    return __Player:set_position(self.player_id, position)
end

---@return Look
function Player:get_look()
    local t = __Player:get_look(self.player_id)
    local look = Look:new(t.yaw, t.pitch)
    return look
end

---@param look Look
---@return nil
function Player:set_look(look)
    return __Player:set_look(self.player_id, look)
end

---@return boolean
function Player:get_on_ground()
    return __Player:get_on_ground(self.player_id)
end

---@param on_ground boolean
---@return nil
function Player:set_on_ground(on_ground)
    return __Player:set_on_ground(self.player_id, on_ground)
end

---@return number
function Player:get_current_slot()
    return __Player:get_current_slot(self.player_id)
end

---@param current_slot number
---@return nil
function Player:set_current_slot(current_slot)
    return __Player:set_current_slot(self.player_id, current_slot)
end

return Player
