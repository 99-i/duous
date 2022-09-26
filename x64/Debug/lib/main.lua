local Client = require("lib._client")
local ServerboundPacket = require("lib._serverbound_packet")


function GetPacket(client_id, packet_data)
    local client = Client:new(client_id)
    local packet = ServerboundPacket:new(packet_data)

    client:get_packet(packet)

end

__duous_packet_listener(GetPacket)
