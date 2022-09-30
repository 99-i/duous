---@class Plugin
Plugin = {}

function Plugin:new(path, json_data)
    self.__index = self
    if json_data["name"] then
        self.name = json_data["name"]
    end
    if json_data["author"] then
        self.author = json_data["author"]
    end
    if json_data["version"] then
        self.version = json_data["version"]
    end
    if not json_data["main"] then
        return
    end
    self.main_file = json_data["main"]
    self.path = path
    return self
end

return Plugin
