local ClientboundPacket = require('lib._clientbound_packet')

local Client = {}

---@alias State
---| "HANDSHAKING"
---| "STATUS"
---| "LOGIN"
---| "PLAY"

function Client:new(client_id)
	self.__index = self
	self.client_id = client_id
	return self
end

---@return State
function Client:get_state()
	return __Client:get_state(self.client_id)
end

---@param state State
function Client:set_state(state)
	return __Client:set_state(self.client_id, state)
end

--#region

---@param packet ServerboundPacket
function Handle_HANDSHAKING_Handshake(packet)
	if packet.fields["Next State"] == 1 then
		Client:set_state("STATUS")
	elseif packet.fields["Next State"] == 2 then
		Client:set_state("LOGIN")
	end
end

---@param packet ServerboundPacket
function Handle_STATUS_Request(packet)
	local response = ClientboundPacket:new(0)
	response:append_string("{ \
			\"version\": { \
				\"name\": \"MCS\", \
				\"protocol\": 5 \
			}, \
			\"players\": { \
				\"max\": 100, \
				\"online\": 5 \
			}, \
			\"description\": { \
				\"text\": \"asdfasdfasdf\" \
			} \
		}")
	Client:send_packet(response)
end

---@param packet ServerboundPacket
function Handle_STATUS_Ping(packet)
	local ping = ClientboundPacket:new(1)
	ping:append_long(packet.fields["Time"])
	Client:send_packet(ping)
end

---@param packet ServerboundPacket
function Handle_LOGIN_LoginStart(packet)
	print("Login start")

	local login_success = ClientboundPacket:new(2)

	login_success:append_string("a75760cd-3c12-4b4c-829a-371aacb4c56a")
	login_success:append_string("ineedmommydommy")

	Client:send_packet(login_success)

	Client:set_state("PLAY")

	local join_game = ClientboundPacket:new(1)

	join_game:append_int(0)
	join_game:append_byte(1)
	join_game:append_byte(0)
	join_game:append_unsigned_byte(0)
	join_game:append_unsigned_byte(10)
	join_game:append_string("default")

	Client:send_packet(join_game)

	local player_abilities = ClientboundPacket:new(0x39)

	player_abilities:append_byte(1)
	player_abilities:append_float(1 / 250)
	player_abilities:append_float(1 / 250)

	Client:send_packet(player_abilities)



	local spawn_position = ClientboundPacket:new(5)
	spawn_position:append_int(0)
	spawn_position:append_int(0)
	spawn_position:append_int(0)

	Client:send_packet(spawn_position)

	local player_position_and_look = ClientboundPacket:new(8)
	player_position_and_look:append_double(0)
	player_position_and_look:append_double(0)
	player_position_and_look:append_double(0)

	player_position_and_look:append_float(0)
	player_position_and_look:append_float(0)

	player_position_and_look:append_boolean(false)

	Client:send_packet(player_position_and_look)

end

---@param packet ServerboundPacket
function Handle_PLAY_KeepAlive(packet)
	print("Keep Alive")
end

---@param packet ServerboundPacket
function Handle_PLAY_ChatMessage(packet)

	print("Chat Message: " .. packet.fields["Message"])

	local chat_message = ClientboundPacket:new(2)

	chat_message:append_string("{\"text\": \"" .. packet.fields["Message"] .. "\"}")

	Client:send_packet(chat_message)

end

---@param packet ServerboundPacket
function Handle_PLAY_UseEntity(packet)
	print("Use Entity")
end

---@param packet ServerboundPacket
function Handle_PLAY_Player(packet)
end

---@param packet ServerboundPacket
function Handle_PLAY_PlayerPosition(packet)
end

---@param packet ServerboundPacket
function Handle_PLAY_PlayerLook(packet)
end

---@param packet ServerboundPacket
function Handle_PLAY_PlayerPositionAndLook(packet)
end

---@param packet ServerboundPacket
function Handle_PLAY_PlayerDigging(packet)
	print("Player Digging")
end

---@param packet ServerboundPacket
function Handle_PLAY_PlayerBlockPlacement(packet)
	print("Player Block Placement")
end

---@param packet ServerboundPacket
function Handle_PLAY_HeldItemChange(packet)
	print("Held Item Change")
end

---@param packet ServerboundPacket
function Handle_PLAY_Animation(packet)
	print("Animation")
end

---@param packet ServerboundPacket
function Handle_PLAY_EntityAction(packet)
	print("Entity Action")
end

---@param packet ServerboundPacket
function Handle_PLAY_SteerVehicle(packet)
	print("Steer Vehicle")
end

---@param packet ServerboundPacket
function Handle_PLAY_CloseWindow(packet)
	print("Close Window")
end

---@param packet ServerboundPacket
function Handle_PLAY_ClickWindow(packet)
	print("Click Window")
end

---@param packet ServerboundPacket
function Handle_PLAY_ConfirmTransaction(packet)
	print("Confirm Transaction")
end

---@param packet ServerboundPacket
function Handle_PLAY_CreativeInventoryAction(packet)
	print("Creative Inventory Action")
end

---@param packet ServerboundPacket
function Handle_PLAY_EnchantItem(packet)
	print("Enchant Item")
end

---@param packet ServerboundPacket
function Handle_PLAY_UpdateSign(packet)
	print("Update Sign")
end

---@param packet ServerboundPacket
function Handle_PLAY_PlayerAbilities(packet)
	print("Player Abilities")
end

---@param packet ServerboundPacket
function Handle_PLAY_TabComplete(packet)
	print("Tab-Complete")
end

---@param packet ServerboundPacket
function Handle_PLAY_ClientSettings(packet)
	print("Client Settings")
end

---@param packet ServerboundPacket
function Handle_PLAY_ClientStatus(packet)
	print("Client Status")
end

---@param packet ServerboundPacket
function Handle_PLAY_PluginMessage(packet)
	print("Plugin Message")
end

---@param packet ServerboundPacket
function Handle_LOGIN_EncryptionResponse(packet)
	print("Encryption Response")
end

--#endregion

local packet_handle_functions = {
	HANDSHAKING = {
		--[[packet id 0]] Handle_HANDSHAKING_Handshake
	},
	STATUS = {
		--[[packet id 0]] Handle_STATUS_Request,
		--[[packet id 1]] Handle_STATUS_Ping
	},
	LOGIN = {
		--[[packet id 0]] Handle_LOGIN_LoginStart,
		--[[packet id 1]] Handle_LOGIN_EncryptionResponse,
	},
	PLAY = {
		--[[packet id 0]] Handle_PLAY_KeepAlive,
		--[[packet id 1]] Handle_PLAY_ChatMessage,
		--[[packet id 2]] Handle_PLAY_UseEntity,
		--[[packet id 3]] Handle_PLAY_Player,
		--[[packet id 4]] Handle_PLAY_PlayerPosition,
		--[[packet id 5]] Handle_PLAY_PlayerLook,
		--[[packet id 6]] Handle_PLAY_PlayerPositionAndLook,
		--[[packet id 7]] Handle_PLAY_PlayerDigging,
		--[[packet id 8]] Handle_PLAY_PlayerBlockPlacement,
		--[[packet id 9]] Handle_PLAY_HeldItemChange,
		--[[packet id 10]] Handle_PLAY_Animation,
		--[[packet id 11]] Handle_PLAY_EntityAction,
		--[[packet id 12]] Handle_PLAY_SteerVehicle,
		--[[packet id 13]] Handle_PLAY_CloseWindow,
		--[[packet id 14]] Handle_PLAY_ClickWindow,
		--[[packet id 15]] Handle_PLAY_ConfirmTransaction,
		--[[packet id 16]] Handle_PLAY_CreativeInventoryAction,
		--[[packet id 17]] Handle_PLAY_EnchantItem,
		--[[packet id 18]] Handle_PLAY_UpdateSign,
		--[[packet id 19]] Handle_PLAY_PlayerAbilities,
		--[[packet id 20]] Handle_PLAY_TabComplete,
		--[[packet id 21]] Handle_PLAY_ClientSettings,
		--[[packet id 22]] Handle_PLAY_ClientStatus,
		--[[packet id 23]] Handle_PLAY_PluginMessage,
	},
}

---@param packet ServerboundPacket
function Client:get_packet(packet)
	packet_handle_functions[self:get_state()][packet.id + 1](packet)
end

---@param packet ClientboundPacket
function Client:send_packet(packet)
	return __Client:send_packet(self.client_id, packet)
end

return Client
