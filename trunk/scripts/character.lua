--[[
-- $Id$
-- Simple character class
-- See Copyright Notice in COPYRIGHT
--]]

-- require"class"

Character = class(function(a, name)
		a.name = name
		a.costumes = {}
		a.costumes.looking = {}
		a.costumes.talking = {}
		a.costumes.moving = {}
		a.costumes.doing = {}
		a.x = 100
		a.y = 100
		a.xspeed = 0.06
		a.yspeed = 0.03
end)


function Character:lookWest()
	self.looking = "west"
end
function Character:lookEast()
	self.looking = "east"
end
function Character:lookNorth()
	self.looking = "north"
end
function Character:lookSouth()
	self.looking = "south"
end

function Character:walkTo(x, y)
	if (x ~= self.x) or (y ~= self.y) then
		self.xdest = x
		self.ydest = y
		self.moving = true
	end
end

function Character:render(delta)
	local costume

	if self.moving then
		local xsign = (self.xdest - self.x)/math.abs(self.xdest - self.x)
		local ysign = (self.ydest - self.y)/math.abs(self.ydest - self.y)

		self.x = self.x + xsign * delta * self.xspeed
		self.y = self.y + ysign * delta * self.yspeed
		if xsign ~= (self.xdest - self.x)/math.abs(self.xdest - self.x) then
			self.x = self.xdest
		end
		if ysign ~= (self.ydest - self.y)/math.abs(self.ydest - self.y) then
			self.y = self.ydest
		end
		if self.x == self.xdest and self.y == self.ydest then
			self.moving = false
		end
		costume = self.costumes.moving[ xsign == 1 and "east" or "west" ]
	elseif self.talking then


	elseif self.doing then


	else	-- standing
		costume = self.costumes.looking[self.looking]
	end

	costume:render(	self.x - costume:getWidth()/2,
					self.y - costume:getHeight() + 3, delta)
end

