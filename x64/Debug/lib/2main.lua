local Client = require("lib._client")
local ServerboundPacket = require("lib._serverbound_packet")
local Plugin = require("lib._plugin")
local lfs = require("lfs")
local json = require("lib._json")
local io = require("io")

---plugins

--first loop through all folders in plugins.
--next, check if each has a valid plugin.json


local plugins = {}

local run_plugins = function()
    for i, v in ipairs(plugins) do
        v.main.on_enable()
    end
end

---@param plugin Plugin
local register_plugin = function(plugin)

    local main = dofile(plugin.path .. "/" .. plugin.main_file)

    plugin.main = main

    register_plugin_main(plugin)
    table.insert(plugins, plugin)

end

---@param plugin_path string
local get_plugin = function(plugin_path)
    local plugin

    local file = io.open(plugin_path .. "/plugin.json", "rb")
    local content = file:read("*a")
    local obj = json.decode(content)

    plugin = Plugin:new(plugin_path, obj)

    if not plugin then
        print("Error creating plugin")
    else
        register_plugin(plugin)
    end



end

local register_all_plugins = function()
    local path = "plugins"
    for file in lfs.dir(path) do


        if not (file == '.' or file == '..') then

            local file_path = path .. '/' .. file
            local type = lfs.attributes(file_path).mode

            if type == 'directory' then
                get_plugin(file_path)


            end

        end
    end
end

print("hi")


local GetPacket = function(client_id, packet_data)
    local client = Client:new(client_id)
    local packet = ServerboundPacket:new(packet_data)

    client:get_packet(packet)
end


__duous_packet_listener(GetPacket)


register_all_plugins()
run_plugins()
