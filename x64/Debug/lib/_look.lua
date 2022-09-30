---@class Look
Look = {}

---@param yaw number
---@param pitch number
function Look:new(yaw, pitch)
    self.__index = self
    self.yaw = yaw
    self.pitch = pitch
    return self
end

return Look
