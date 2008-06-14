--[[
-- $Id$
-- Simple animation class
-- See Copyright Notice in COPYRIGHT
--]]

-- require"class"

Animation = class(function(a)
	a.running = true
	a.frames = {}
	a.activeFrame = 1
	a.time = 0
	a.duration = 0
end)

function Animation:addFrame(image, x, y, width, height, delay)
	self.frames[#self.frames+1] = {
		image = image,
		x = x,
		y = y,
		width = width,
		height = height,
		delay = delay
	}
	self.duration = self.duration + delay
	return self
end

function Animation:addFrames(image, sizex, sizey, width, height, sep, delay)
	for i=1, sizey do
		for j=1, sizex do
			self.frames[#self.frames+1] = {
				image = image,
				x = (j-1)*(width+sep),
				y = (i-1)*(height+sep),
				width = width,
				height = height,
				delay = delay
			}
		end
	end
	self.duration = sizex * sizey * delay
	return self
end

function Animation:getWidth()
	return self.frames[self.activeFrame].width or 0
end

function Animation:getHeight()
	return self.frames[self.activeFrame].height or 0
end

function Animation:isTransparent(x, y)
	local frame = self.frames[self.activeFrame]
	x = x + frame.x
	y = y + frame.y
	return frame.image:isTransparent(x, y)
end

function Animation:start()
	self.running = true
end

function Animation:stop()
	self.running = false
	self.activeFrame = 1
end

function Animation:render(x, y, delta)
	if self.running then
		self.time = math.fmod(self.time + delta, self.duration)
	end
	local moment = 0
	for i,v in ipairs(self.frames) do
		moment = moment + v.delay
		if moment >= self.time then
			self.activeFrame = i
			v.image:render{x, y, rect={v.x, v.y, v.width, v.height}}
			break
		end
	end
end
