--[[
-- $Id$
-- Font class to ease usage of TTF fonts
-- See Copyright Notice in COPYRIGHT
--]]

-- require"class"

Font = class(function(a, font, size)
	local font = scrupp.addFont(font, size)	
	a.font = font
	a.height = font:getHeight()
	a.lineSkip = font:getLineSkip()	
	a.chars = {}
end)

-- begin: wrapping the built-in functions for fonts
function Font:getTextSize(text)
	return self.font:getTextSize(text)
end

function Font:getHeight()
	return self.height
end

function Font:getLineSkip()
	return self.lineSkip
end

function Font:generateImage(arg)
	return self.font:generateImage(arg)
end
-- end: wrapping the built-in functions for fonts

function Font:cache(str)
	local str = str or "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!?()[]{}.,;:-_"
	
	local font = self.font
	local x -- x-coordinate of the next inserted char
	local char
	local chars = self.chars
	local width, height
	local defaultImage = font:generateImage(str) 
	
	for i=1,str:len() do
		char = str:sub(i,i)
		width, height = font:getTextSize(char)
		x = font:getTextSize(str:sub(1,i-1))		
		chars[char] = {
			0, 0, -- placeholders for the x- and y-coordinates used for rendering
			image = defaultImage,
			rect = {x, 0, width, height},
			color = {255, 255, 255},
			width = width
		}
	end
end

function Font:print(x, y, color, text)
	local chars = self.chars
	local char, charTable
	local orig_x = x
	local font = self.font
	text = tostring(text)
	
	for i=1,text:len() do
		char = text:sub(i,i)
		if char == "\n" then
			x = orig_x
			y = y + self.lineSkip			
		else			
			if not chars[char] then
				chars[char] = {
					0, 0, -- placeholders for the x- and y-coordinates used for rendering
					image = font:generateImage(char),
					width = font:getTextSize(char)
				}
			end
			charTable = chars[char]
			charTable[1] = x
			charTable[2] = y
			charTable.color = color
			charTable.image:render(charTable)
			x = x + charTable.width
		end
	end
end
