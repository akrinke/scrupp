--[[
-- $Id$
-- Font class to ease usage of TTF fonts
-- See Copyright Notice in COPYRIGHT
--]]

-- require"class"

Font = class(function(self, font, size)
	font = scrupp.addFont(font, size)	
	self.font = font
	self.color = {255, 255, 255}
	self.height = font:getHeight()
	self.lineSkip = font:getLineSkip()
	-- table containing everything needed to render each glyph
	self.chars = {}
	-- used for kerning calculations
	self.chars[""] = {width = 0}
	-- table containing the (cached) kerning values
	self.kerning = {}
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

function Font:setColor(color)
	self.color = color
end

function Font:cache(str)
	local str = str
	if type(str) == string then
		str = {str}
	else
		str = {	"A B C D E F G H I J K L M N O P Q R S T U V W X Y Z",
				"a b c d e f g h i j k l m n o p q r s t u v w x y z",
				"0 1 2 3 4 5 6 7 8 9",
				"! \" # $ % & ' ( ) * + , - . / : ; < = > ? @ [ \\ ] ^ _ ` { | } ~"}
	end

	local font = self.font
	local x -- x-coordinate of the next inserted char
	local char
	local chars = self.chars
	local width, height
	local image
	local i, next_i, byte

	for _,s in ipairs(str) do
		image = font:generateImage(s)
		i = 1
		next_i = 1
		while i <= s:len() do
			byte = s:byte(i)
			if byte < 128 then
				-- nothing to do - it's ASCII
			elseif byte < 224 then
				next_i = i + 1
			elseif byte < 240 then
				next_i = i + 2
			else
				next_i = i + 3
			end
			char = s:sub(i, next_i)
			x = font:getTextSize(s:sub(1,i-1))
			next_i = next_i + 2
			i = next_i

			width, height = font:getTextSize(char)
			chars[char] = {
				0, 0, -- placeholders for the x- and y-coordinates used for rendering
				image = image,
				rect = {x, 0, width, height},
				color = {255, 255, 255},
				width = width
			}
		end
	end
end

function Font:print(x, y, ...)
	local chars = self.chars
	local kerning = self.kerning
	local char, charTable
	local orig_x = x
	local font = self.font
	local text
	local i = 1
	local next_i = 1
	local last_char = ""

	for n=1, select("#", ...) do
		text = tostring(select(n, ...))
		while i <= text:len() do
			byte = text:byte(i)
			if byte < 128 then
				-- nothing to do - it's ASCII
			elseif byte < 224 then
				next_i = i + 1
			elseif byte < 240 then
				next_i = i + 2
			else
				next_i = i + 3
			end
			char = text:sub(i, next_i)
			next_i = next_i + 1
			i = next_i

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
				if not kerning[last_char] then
					kerning[last_char] = {}
				end
				if not kerning[last_char][char] then
					kerning[last_char][char] = 
						chars[last_char].width + chars[char].width
						- font:getTextSize(last_char..char)
					--print(last_char, char, kerning[last_char][char])
				end				
				charTable[1] = x - kerning[last_char][char]
				charTable[2] = y
				charTable.color = self.color
				charTable.image:render(charTable)
				x = charTable[1] + charTable.width
				last_char = char
			end
		end
	end
end
