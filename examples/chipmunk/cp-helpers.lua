--[[
-- $Id$
-- This file contains helper functions for werechipmunk, the Lua library we
-- use as interface to the physics library chipmunk.
-- The graphics for the objects are created on the fly using Cairo.
-- See Copyright Notice in COPYRIGHT
--]]

local cp    = require "werechip"
local cairo = require "oocairo"
local white = require "color".white

local M = {}

function M.generateCircleShapeImage(shape)
	local r = shape:getRadius()

	local surface = cairo.image_surface_create("argb32", 2*r+4, 2*r+4)
	local cr = cairo.context_create(surface)

	cr:move_to(r+2, r+2)
	cr:arc(r+2, r+2, r, 3.5 * math.pi, 5.5 * math.pi)

	cr:set_line_width(2)
	cr:set_source_rgb(1, 1, 1)
	cr:fill_preserve()
	cr:set_source_rgb(0, 0, 0);
	cr:stroke()

	local image = scrupp.addImageFromCairo(surface)
	image:setCenter(r+2, r+2)

	return image
end

function M.generateSegmentShapeImage(shape)
	local r = shape:getRadius()
	local ax, ay = shape:getA()
	local bx, by = shape:getB()
	local w = math.abs(ax - bx)
	local h = math.abs(ay - by)

	if r == 0 then r = 1 end

	local surface = cairo.image_surface_create("argb32", w+2*r+4, h+2*r+4)
	local cr = cairo.context_create(surface)
	
	cr:set_line_width(2*r)
	cr:move_to(r+2, r+2)
	cr:rel_line_to(w, h)
	cr:stroke()

	cr:set_line_width(1)
	cr:arc(r+2, r+2, r, 0, 2 * math.pi)
	cr:arc(w+r+2, h+r+2, r, 0, 2 * math.pi)
	cr:fill()
	cr:stroke()

	local image = scrupp.addImageFromCairo(surface)
	image:setCenter(-ax+r+2, -ay+r+2)

	return image
end

function M.generatePolyShapeImage(shape)
	local count = shape:getNumVerts()
	local maxX = -math.huge
	local maxY = maxX
	local minX = math.huge
	local minY = minX
	local x, y

	-- get the minimal and maximal x- and y-coordinate of the vectors
	-- store it in maxX, minX, maxY, minY
	for i=1,count do
		x, y = shape:getVert(i)
		if x>maxX then 
			maxX = x
		end
		if x<minX then 
			minX = x 
		end
		if y>maxY then
			maxY = y
		end
		if y<minY then
			minY = y
		end
	end

	-- create a surface slightly larger than the width and height of the shape
	-- width of the shape: maxX - minX
	-- height of the shape: maxY - minY
	local surface = cairo.image_surface_create("argb32", maxX-minX+4, maxY-minY+4)
	local cr = cairo.context_create(surface)

	cr:set_line_width(2)
	cr:translate(-minX+2, -minY+2)
	for i=1,count do
		x, y = shape:getVert(i)
		if i==1 then
			cr:move_to(x, y)
		end
		if i>1 then
			cr:line_to(x, y)
		end
		if i==count then
			cr:close_path()
		end
	end
	cr:set_source_rgb(1, 1, 1)
	cr:fill_preserve()
	cr:set_source_rgb(0, 0, 0);
	cr:stroke()

	local image = scrupp.addImageFromCairo(surface)
	image:setCenter(-minX+2, -minY+2)

	return image
end

local GRABABLE_MASK_BIT = math.ldexp(1, 31)
M.GRABABLE_MASK_BIT = GRABABLE_MASK_BIT
local NOT_GRABABLE_MASK = GRABABLE_MASK_BIT - 1
M.NOT_GRABABLE_MASK = NOT_GRABABLE_MASK

local mousePointX, mousePointY = 0, 0
local lastMousePointX, lastMousePointY = 0, 0
local mouseBody = cp.newStaticBody()
local mouseJoint

function M.generateCallbacks(demo)
	-- white background
	local bg = {color = white, fill = true, 0, 0, 640, 0, 640, 480, 0, 480}

	local t = {}
	function t.render(dt)
		demo.space:step(1/70)
		demo.space:step(1/70)
		demo.space:step(1/70)

		scrupp.draw(bg)
		demo:render()
		
		mousePointX, mousePointY = scrupp.getMousePos()
		local newPointX, newPointY = cp.vect.lerp(lastMousePointX, lastMousePointY, mousePointX, mousePointY, 0.25)
		mouseBody:setPosition(newPointX, newPointY)
		mouseBody:setVelocity((newPointX - lastMousePointX) * 60, (newPointY - lastMousePointY) * 60)
		lastMousePointX, lastMousePointY = newPointX, newPointY
	end

	function t.mousepressed(x, y, btn)
		if btn == "left" then
			local shape = demo.space:pointQueryFirst(x, y, GRABABLE_MASK_BIT, 0)
			if shape then
				local body = shape:getBody()
				mouseJoint = cp.newPivotJoint(mouseBody, body, 0, 0, body:getWorld2Local(x, y))
				mouseJoint:setMaxForce(50000)
				mouseJoint:setBiasCoefficient(0.15)
				demo.space:addConstraint(mouseJoint)
			end
		end
	end

	function t.mousereleased(x, y, btn)
		if mouseJoint then
			demo.space:removeConstraint(mouseJoint)
			mouseJoint = nil
		end
	end

	function t.keypressed(key)
		if     key == "RETURN" or key == "KP_ENTER" then
			demo = require(demos[2])
			demo:init()
		elseif key == "ESCAPE" then
			scrupp.exit()
		end
	end

	return t
end

return M

