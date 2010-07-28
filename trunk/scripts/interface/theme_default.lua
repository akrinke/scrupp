--[[
-- $Id$
-- Default theme for the Interface library
-- See Copyright Notice in COPYRIGHT
--]]

local pi = math.pi
local unpack = unpack
local scrupp = scrupp
local Cairo = require "oocairo"

module(...)

-- button default options
local shadowRadius = 1
local cornerRadius = 8
local borderColor = {0.749, 0.749, 0.749}
local gradients = {
	normal 		= {0.9, 0.82, 0.95},
	disabled	= {0.7, 0.62, 0.75},
	pressed		= {0.8, 0.72, 0.85},
	focused		= {0.9, 0.85, 0.98}	
}

-- local functions

local function createOutline(cr, x1, y1, x2, y2)
	cr:set_line_width(1)
	cr:translate(0.5, 0.5)
	cr:move_to(x1+cornerRadius, y1)
	cr:line_to(x2-cornerRadius-shadowRadius, 0)
	cr:arc(x2-cornerRadius-shadowRadius, cornerRadius, cornerRadius, 1.5*pi, 2*pi)
	cr:line_to(x2-shadowRadius, y2-cornerRadius-shadowRadius)
	cr:arc(x2-cornerRadius-shadowRadius, y2-cornerRadius-shadowRadius, cornerRadius, 0, 0.5*pi)
	cr:line_to(cornerRadius, y2-shadowRadius)
	cr:arc(cornerRadius, y2-cornerRadius-shadowRadius, cornerRadius, 0.5*pi, pi)
	cr:line_to(x1, y1+cornerRadius)
	cr:arc(x1+cornerRadius, y1+cornerRadius, cornerRadius, pi, 1.5*pi)	
end

local function fillOutline(cr, x1, y1, x2, y2, style)
	local pat = Cairo.pattern_create_linear(x1, y1, x1, y2)
	local grd = gradients[style]
	pat:add_color_stop_rgb(0,         1,      1,      1)
	pat:add_color_stop_rgb(0.43, grd[1], grd[1], grd[1])
	pat:add_color_stop_rgb(0.5,  grd[2], grd[2], grd[2])
	pat:add_color_stop_rgb(1,    grd[3], grd[3], grd[3])
	cr:set_source(pat)
	cr:fill_preserve()
end

-- global functions

-- style is one of "normal", "disabled", "pressed", "focussed"
function createButtonImage(width, height, style)
	local surface = Cairo.image_surface_create("argb32", width, height)
	local cr = Cairo.context_create(surface)
	-- create outline path
	createOutline(cr, 0, 0, width, height)
	-- fill outline path
	fillOutline(cr, 0, 0, width, height, style)
	-- stroke outline path (draw border)
	cr:set_source_rgb(unpack(borderColor))
	cr:stroke()
	return scrupp.addImageFromCairo(surface)
end
