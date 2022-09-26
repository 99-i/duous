---@class ServerboundPacket
local ServerboundPacket = {}

function ServerboundPacket:new(packet_data)
    assert(packet_data.direction == "SERVERBOUND", "ServerboundPacket has to be Serverbound!")
    self.__index = self
    self.id = packet_data.id
    self.name = packet_data.name
    self.fields = packet_data.fields
    return self
end

return ServerboundPacket
