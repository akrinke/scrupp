--[[
-- $Id$
-- Support class for loading and saving application data on the users pc
-- See Copyright Notice in COPYRIGHT
--]]

require "class"
require "lfs"

local function get_path(name)
	if type(name) ~= "string" then
		return nil, "need string as argument"
	end
	local unix_name = name:lower()
	unix_name = string.gsub(unix_name, "[^%a%d]", "_")

	local home = os.getenv("HOME")
	local appdata = os.getenv("APPDATA")

	local path
	if scrupp.PLATFORM == "Linux" then
		if not home then
			return nil, "HOME environment variable not set"
		end
		path = home .. "/." .. unix_name
	elseif scrupp.PLATFORM == "Mac OS X" then
		if not home then
			return nil, "HOME environment variable not set"
		end
		path = home .. "/Library/Application Support/" .. name
	elseif scrupp.PLATFORM == "Windows" then
		if not appdata then
			return nil, "APPDATA environment variable not set"
		end
		path = appdata .. "\\" .. name
	end

	return path
end

Appdir = class(function(self, name)
	self.name = name
	self.path = assert(get_path(name))
end)

function Appdir:getPath()
	return self.path
end

function Appdir:exists()
	return(lfs.attributes(self.path) ~= nil)
end

function Appdir:create()
	if not self:exists() then
		assert(lfs.mkdir(self.path))
	end
end

function Appdir:isWritable()
	math.randomseed(os.time())
	local random_file = self.path .. string.sub(package.config, 1, 1) .. math.random(9999, 999999)
	local result, msg = io.open(random_file, "w")
	if not result then
		return nil, msg .. "; directory not writable"
	end
	os.remove(random_file)
	return true
end

function Appdir:validate()
	self:create()
	assert(self:isWritable())
end
