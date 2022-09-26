---@class Position
Position = {}

function Position:new(x, y, z)
    self.__index = self
    self.x = x
    self.y = y
    self.z = z

    return self
end

return Position
