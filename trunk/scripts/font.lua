--[[
-- $Id$
-- Font class to ease usage of TTF fonts
-- See Copyright Notice in COPYRIGHT
--]]

-- require"class"

Font = class(function(a, font, size)
	local str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!?()[]{}.,;:-_"
			
	local x -- x-coordinate of the next inserted char
	local char
	local chars = {}
	local width, height
	
	font = scrupp.addFont(font, size)
	local defaultImage = font:generateImage(str) 
	
	a.font = font
	a.lineSkip = font:getLineSkip()
	
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
	
	a.chars = chars
end)

function Font:print(x, y, color, text)
	local chars = self.chars
	local char, charTable
	local orig_x = x
	local font = self.font
	
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
