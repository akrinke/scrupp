--[[
-- $Id$
-- Simple scene class
-- See Copyright Notice in COPYRIGHT
--]]

-- require"class"

Scene = class(function(a)


end)

--~ function Scene:setBG(image)
--~ 	self.background = image
--~ end

--~ function Scene:setFG(image)
--~ 	self.foreground = image
--~ end

function Scene:mousepressed(x, y, button)
	if button == "left" then
		local walk, xnew, ynew = self:makeWalkable(x, y)
		if walk then
			self.characters[1]:walkTo(xnew, ynew)
		end
	end
end
--~ 		local coordList = {}
--~ 		local openList = {}
--~ 		local p	-- active point
--~ 		local index	--index of activePoint in openList
--~
--~ 		openList[#openList+1] = {	x = self.characters[1].x,
--~ 									y = self.characters[1].y,
--~ 									F = 0 }
--~
--~ 		while(true) do
--~ 			p = { F = math.huge }
--~ 			-- finding the point with the lowest F-value (activePoint)
--~ 			for i=1, #openList do
--~ 				if openList[i].F < p.F then
--~ 					p = openList[i]
--~ 					index = i
--~ 				end
--~ 			end
--~ 			if p.F == math.huge then
--~ 				print("no path found")
--~ 				break
--~ 			end
--~ 			if (p.x == x) and (p.y == y) then
--~ 				print("goal reached")
--~ 				break
--~ 			end
--~ 			-- moving activePoint to the closed list
--~ 			coordList[p.y] = coordList[p.y] or {}
--~ 			coordList[p.y][p.x] = 'c' -- point is closed
--~ 			openList[i] = openList[#openList]
--~ 			openList[#openList] = nil
--~
--~ 			-- list of points
--~ 			local points = {
--~ 				{ x = p.x - 1,	y = p.y - 1 },
--~ 				{ x = p.x,		y = p.y - 1 },
--~ 				{ x = p.x + 1,	y = p.y - 1 },
--~ 				{ x = p.x - 1,	y = p.y		},
--~ 				{ x = p.x + 1,	y = p.y	 	},
--~ 				{ x = p.x - 1,	y = p.y + 1 },
--~ 				{ x = p.x,		y = p.y + 1 },
--~ 				{ x = p.x - 1,	y = p.y + 1 }
--~ 			}
--~
--~ 			for i,p in ipairs(points) do
--~ 				-- is point already in closed list? -> break
--~ 				if 	coordList[p.y] and coordList[p.y][p.x] == 'c' then break end
--~ 				-- is point not walkable? -> break
--~ 				if self.walkableAreas:isTransparent(p.x, p.y) then break end
--~ 				-- is point outside of screen? -> break
--~ 				if 	p.x < 0 or p.x >= game:getDisplayWidth() or
--~ 					p.y < 0 or p.y >= game:getDisplayHeight() then break end
--~
--~ 				-- is point not in open list? -> add to open list
--~ 				coordList[p.y] = coordList[p.y] or {}
--~ 				if coordList[p.y][p.x] == 'o' then
--~
--~
--~
--~
--~ 			end
--~
--~ 		until 	(activePoint.F == math.huge) or
--~ 				(activePoint.x == x) and (activePoint.y == y)
--~

--~
--~ 	end
--~ end

-- (y - y0) (x1 - x0) - (x - x0) (y1 - y0)
--~ function Scene:isWalkable(x, y)
--~ 	local tab = self.walkableAreas
--~ 	if type(tab == "table") then
--~ 		for _,t in ipairs(tab) do
--~ 			print("---------")
--~ 			for i=1, #t/2 - 1 do
--~ 				print((y-t[2*i])*(t[2*i+1]-t[2*i-1])-(x-t[2*i-1])*(t[2*i+2]-t[2*i]))
--~ 			end
--~ 			print("-----")
--~ 		end
--~ 		return false
--~ 	else
--~ 		return true
--~ 	end
--~ end

--~     int pnpoly(int npol, float *xp, float *yp, float x, float y)
--~     {
--~       int i, j, c = 0;
--~       for (i = 0, j = npol-1; i < npol; j = i++) {
--~         if ((((yp[i] <= y) && (y < yp[j])) ||
--~              ((yp[j] <= y) && (y < yp[i]))) &&
--~             (x < (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))
--~           c = !c;
--~       }
--~       return c;
--~     }

function Scene:isWalkable(x, y)
	local tab = self.walkableAreas
	if type(tab) == "table" then
		for _,t in ipairs(tab) do
			local i,j,c = 1, #t/2, false
			while (i < #t/2+1) do
				local xi, yi, xj, yj = t[2*i-1], t[2*i], t[2*j-1], t[2*j]
				if ( (((yi <= y) and (y < yj)) or ((yj <= y) and (y < yi)))
					 and (x < ((xj-xi)*(y-yi)/(yj-yi)+xi)) ) then
					c = not c
				end
				j = i
				i = i+1
			end
			if c then
				return true
			end
		end
	end
	return false
end

function Scene:makeWalkable(x, y)
	return self:isWalkable(x, y), x, y
end
