--[[
-- $Id$
-- Simple object class
-- See Copyright Notice in COPYRIGHT
--]]

-- require"class"

Object = class(function(a, name, image)
	a.name = name
	a.image = image
	a.x = 0
	a.y = 0
	a.width = image and image:getWidth() or 0
	a.height = image and image:getHeight() or 0
end)

function Object:render(delta)
	self.image:render(self.x, self.y, delta)
end

function Object:inside(x, y)
	if 	(x>=self.x) and (x<self.x+self.width) and
		(y>=self.y) and (y<self.y+self.height) then
		-- mouse inside object ;)
		return not self.image:isTransparent(x-self.x, y-self.y)
	else
		return false
	end
end
