--[[
-- $Id$
-- Simple event class; Originally written by Stephan Bruny
-- See Copyright Notive in COPYRIGHT
--]]

require "class"

Event = class(function(self)
	self.callbacks = {}
end)

function Event:add(callback)
	if type(callback) == 'function' then
		table.insert(self.callbacks, callback)
	else
		error('Given callback is not a function ('..type(callback)..')')
	end
end

function Event:remove(callback)
	if type(callback) ~= 'function' then
		error('Given callback is not a function ('..type(callback)..')')
	end
	
	for key, value in ipairs(self.callbacks) do
		if (value == callback) then
			table.remove(self.callbacks, key)
			return true
		end
	end
	return false
end

function Event:clear()
	self.callbacks = {}
end

function Event:__call(...)
	for key, value in ipairs(self.callbacks) do
		value(...)
	end
end
