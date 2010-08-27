--[[
-- $Id$
-- Simple event class; Originally written by Stephan Bruny
-- See Copyright Notice in COPYRIGHT
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

-- a call to the event object triggers the event
-- the last non-nil return value of the callbacks
-- becomes the return value of the call
function Event:__call(...)
	local retval = nil
	for key, value in ipairs(self.callbacks) do
		retval = value(...) or retval
	end
	return retval
end

function Event.install()
	-- create an event for every possible
	-- callback from Scrupp
	onUpdate       = Event()
	onRender       = Event()
	onResize       = Event()
	onKeyPress     = Event()
	onKeyRelease   = Event()
	onMousePress   = Event()
	onMouseRelease = Event()
	-- redefine the main table
	main = {
		update = function(...)
			return onUpdate(...)
		end,
		
		render = function(...)
			return onRender(...)
		end,
		
		resized = function(...)
			return onResize(...)
		end,
		
		keypressed = function(...)
			return onKeyPress(...)
		end,
		
		keyreleased = function(...)
			return onKeyRelease(...)
		end,
		
		mousepressed = function(...)
			return onMousePress(...)
		end,
		
		mousereleased = function(...)
			return onMouseRelease(...)
		end	
	}
end
