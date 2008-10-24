--[[
-- $Id$
-- Simple timer class
-- See Copyright Notice in COPYRIGHT
--]]

-- require"class"

Timer = class(function(a, duration, callback)
	a.duration = duration
	a.startTicks = 0
	a.elapsedTicks = 0
	a.running = false
	a.callback = callback
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
	