--[[
-- $Id$
-- Simple timer class
-- See Copyright Notice in COPYRIGHT
--]]

require "class"

Timer = class(function(self, duration, callback)
	self.duration = duration
	self.startTicks = 0
	self.elapsedTicks = 0
	self.running = false
	self.callback = callback
end)

function Timer:start()
	self.startTicks = scrupp.getTicks() - self.elapsedTicks
	self.running = true
end

function Timer:stop()
	if self.running then
		self.elapsedTicks = scrupp.getTicks() - self.startTicks
		self.running = false
	end
end

function Timer:reset()
	self.running = false
	self.elapsedTicks = 0
end

function Timer:update()
	if self.running then
		local ticks = scrupp.getTicks()
		if ticks - self.startTicks >= self.duration then
			self.startTicks = ticks
			self.elapsedTicks = 0
			self.callback(self)
		end
	end
end
	