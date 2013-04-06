--[[
-- $Id$
-- Simple animation class
-- See Copyright Notice in COPYRIGHT
--]]

require "class"

Animation = class(function(self)
	self.running = true
	self.frames = {}
	self.activeFrame = 1
	self.time = 0
	self.duration = 0
end)

function Animation:addFrame(image, x, y, width, height, delay)
	if type(image) == "string" then
		image = scrupp.addImage(image)
	end
	
	self.frames[#self.frames+1] = {
		image = image,
		rect = { x, y, width, height },
		delay = delay
	}
	self.duration = self.duration + delay
	return self
end

function Animation:addFrames(image, sizex, sizey, width, height, sep, delay)
	if type(image) == "string" then
		image = scrupp.addImage(image)
	end
	
	for i=1, sizey do
		for j=1, sizex do
			self.frames[#self.frames+1] = {
				image = image, 
				rect = {
					(j-1)*(width+sep), 	-- x
					(i-1)*(height+sep),	-- y
					width,
					height
				},
				delay = delay
			}
		end
	end
	self.duration = sizex * sizey * delay
	return self
end

function Animation:getWidth()
	return self.frames[self.activeFrame].rect[3] or 0
end

function Animation:getHeight()
	return self.frames[self.activeFrame].rect[4] or 0
end

function Animation:getSize()
	return 	self.frames[self.activeFrame].rect[3] or 0,
			self.frames[self.activeFrame].rect[4] or 0
end

function Animation:isTransparent(x, y)
	local frame = self.frames[self.activeFrame]
	x = x + frame.rect[1]
	y = y + frame.rect[2]
	return frame.image:isTransparent(x, y)
end

function Animation:start()
	self.running = true
end

function Animation:stop()
	self.running = false
	self.activeFrame = 1
end

function Animation:copy()
	local t = {}
	t.running = self.running
	t.frames = self.frames
	t.activeFrame = self.activeFrame
	t.time = self.time
	t.duration = self.duration
	setmetatable(t, getmetatable(self))
	
	return t
end

function Animation:render(x, y, delta)
	if self.running then
		self.time = math.fmod(self.time + delta, self.duration)
	end
	local moment = 0
	for i,frame in ipairs(self.frames) do
		moment = moment + frame.delay
		if moment >= self.time then
			self.activeFrame = i
			frame.x = x
			frame.y = y
			frame.image:render(frame)
			break
		end
	end
end
