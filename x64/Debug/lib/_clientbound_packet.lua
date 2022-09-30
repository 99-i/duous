---@class ClientboundPacket
ClientboundPacket = {}

function ClientboundPacket:new(id)
    self.__index = self
    self.id = id
    self.values = {}
    self.types = {}
    return self
end

---@param boolean boolean
function ClientboundPacket:append_boolean(boolean)
    table.insert(self.values, boolean)
    table.insert(self.types, "boolean")
end

---@param byte number
function ClientboundPacket:append_byte(byte)
    table.insert(self.values, byte)
    table.insert(self.types, "byte")
end

---@param unsigned_byte number
function ClientboundPacket:append_unsigned_byte(unsigned_byte)
    table.insert(self.values, unsigned_byte)
    table.insert(self.types, "unsigned_byte")
end

---@param short number
function ClientboundPacket:append_short(short)
    table.insert(self.values, short)
    table.insert(self.types, "short")
end

---@param unsigned_short number
function ClientboundPacket:append_unsigned_short(unsigned_short)
    table.insert(self.values, unsigned_short)
    table.insert(self.types, "unsigned_short")
end

---@param int number
function ClientboundPacket:append_int(int)
    table.insert(self.values, int)
    table.insert(self.types, "int")
end

---@param long number
function ClientboundPacket:append_long(long)
    table.insert(self.values, long)
    table.insert(self.types, "long")
end

---@param float number
function ClientboundPacket:append_float(float)
    table.insert(self.values, float)
    table.insert(self.types, "float")
end

---@param double number
function ClientboundPacket:append_double(double)
    table.insert(self.values, double)
    table.insert(self.types, "double")
end

---@param string string
function ClientboundPacket:append_string(string)
    table.insert(self.values, string)
    table.insert(self.types, "string")
end

---@param varint number
function ClientboundPacket:append_varint(varint)
    table.insert(self.values, varint)
    table.insert(self.types, "varint")
end

---@param varlong number
function ClientboundPacket:append_varlong(varlong)
    table.insert(self.values, varlong)
    table.insert(self.types, "varlong")
end

---@param position Position
function ClientboundPacket:append_position(position)
    table.insert(self.values, position)
    table.insert(self.types, "position")
end

---@param angle number
function ClientboundPacket:append_angle(angle)
    table.insert(self.values, angle)
    table.insert(self.types, "angle")
end

--TODO: append_uuid
--TODO: append_byte_array

return ClientboundPacket
