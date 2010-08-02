--[[
-- $Id$
-- Interface for Scrupp; by: Everton Costa
-- Tooltip class
-- See Copyright Notice in COPYRIGHT
--]]

require "class"
require "interface.utils"

local split = interface.utils.split
local findDigitAndReplace = interface.utils.findDigitAndReplace

local Tip = class(function(self, image, font)
	self.image = nil
	self.hasImage = false
	self.alpha = 255
	self.imagePath = ""
	self.borderColor = { 0, 0, 0, 255}
	self.bgcolor = { 180, 180, 180, 180 }
	self.fgColor = { 0, 0, 0, 255}
	self.font = nil
	self.showBorder = true
	self.isShowing = false
	self.dimension = {}
	if image~=nil then
		image = scrupp.addImage(image)
		self.image = image
		self.hasImage = true
		self.image:setAlpha(self.alpha)
		self.dimension = { self.image:getWidth(), self.image:getHeight() }
	end
	if font==nil then
		self.font = scrupp.addFont('fonts/Vera.ttf', 11)
		self.fontPath = 'fonts/Vera.ttf'
		self.fontName = 'Vera.ttf'
		self.fontSize = 11
	else
		font[1] = string.lower(findDigitAndReplace(font[1],"\\","/"))
		if split(font[1],'.')[table.maxn(split(font[1],'.'))]~='ttf' and split(font[1],'.')[table.maxn(split(font[1],'.'))]~='fon' then return end
		self.fontPath = newFontPath
		self.fontName = split(font[1],'/')[table.maxn(split(font[1],'/'))]
		if type(font[2])~='number' then font[2]=11 end
		if font[2]<8 or font[2]>25 then font[2]=11 end
		self.font = scrupp.addFont(font[1], font[2])
		self.fontSize = font[2]
	end
end)

function Tip:remove()
	self.image = nil
	self.hasImage = false
	self.alpha = 255
	self.imagePath = ""
	self.fontPath = ""
	self.fontSize = ""
	self.font = nil
	self = nil
end

function Tip:setImage(newImage)
	if newImage == nil then self.hasImage = false self.image = nil self.imagePath = "" return end
	self.image = scrupp.addImage(newImage)
	self.hasImage = true
	self.imagePath = newImage
	self.image:setAlpha(self.alpha)
	self.dimension = { self.image:getWidth(), self.image:getHeight() }
end

function Tip:getImage()
	return self.imagePath
end

function Tip:setAlpha(newAlpha)
	if type(newAlpha)~='number' then return end
	if newAlpha >= 0 and newAlpha <= 255 then
		self.alpha = newAlpha
	else
		self.alpha = 255 --default
	end
	if self.image ~= nil then
		self.image:setAlpha(self.alpha)
	end
end

function Tip:getAlpha()
	return self.alpha
end

function Tip:setFgColor(newColor)
	local colorTable = {0 ,0 , 0, 255 }
	local colorInvalid = false
	if type(newColor[1])=='number' then
		if newColor[1]>=0 and newColor[1]<=255 then
			colorTable[1] = newColor[1]
		else
			colorTable[1] = 0
		end
	else
		colorInvalid = true
	end
	if type(newColor[2])=='number' then
		if newColor[2]>=0 and newColor[2]<=255 then
			colorTable[2] = newColor[2]
		else
			colorTable[2] = 0
		end
	else
		colorInvalid = true
	end
	if type(newColor[3])=='number' then
		if newColor[3]>=0 and newColor[3]<=255 then
			colorTable[3] = newColor[3]
		else
			colorTable[3] = 0
		end
	else
		colorInvalid = true
	end
	if type(newColor[4])=='number' then
		if newColor[4]>=0 and newColor[4]<=255 then
			--0 is totally trasparent
			colorTable[4] = newColor[4]
		else
			colorTable[4] = 255
		end
	end
	if colorInvalid then return end
	self.fgColor = colorTable
end

function Tip:setBgColor(newColor)
	local colorTable = {0 ,0 , 0, 255 }
	local colorInvalid = false
	if type(newColor[1])=='number' then
		if newColor[1]>=0 and newColor[1]<=255 then
			colorTable[1] = newColor[1]
		else
			colorTable[1] = 0
		end
	else
		colorInvalid = true
	end
	if type(newColor[2])=='number' then
		if newColor[2]>=0 and newColor[2]<=255 then
			colorTable[2] = newColor[2]
		else
			colorTable[2] = 0
		end
	else
		colorInvalid = true
	end
	if type(newColor[3])=='number' then
		if newColor[3]>=0 and newColor[3]<=255 then
			colorTable[3] = newColor[3]
		else
			colorTable[3] = 0
		end
	else
		colorInvalid = true
	end
	if type(newColor[4])=='number' then
		if newColor[4]>=0 and newColor[4]<=255 then
			--0 is totally trasparent
			colorTable[4] = newColor[4]
		else
			colorTable[4] = 255
		end
	end
	if colorInvalid then return end
	self.bgColor = colorTable
end

function Tip:setBorderColor(newColor)
	local colorTable = {0 ,0 , 0, 255 }
	local colorInvalid = false
	if type(newColor[1])=='number' then
		if newColor[1]>=0 and newColor[1]<=255 then
			colorTable[1] = newColor[1]
		else
			colorTable[1] = 0
		end
	else
		colorInvalid = true
	end
	if type(newColor[2])=='number' then
		if newColor[2]>=0 and newColor[2]<=255 then
			colorTable[2] = newColor[2]
		else
			colorTable[2] = 0
		end
	else
		colorInvalid = true
	end
	if type(newColor[3])=='number' then
		if newColor[3]>=0 and newColor[3]<=255 then
			colorTable[3] = newColor[3]
		else
			colorTable[3] = 0
		end
	else
		colorInvalid = true
	end
	if type(newColor[4])=='number' then
		if newColor[4]>=0 and newColor[4]<=255 then
			--0 is totally trasparent
			colorTable[4] = newColor[4]
		else
			colorTable[4] = 255
		end
	end
	if colorInvalid then return end
	self.borderColor = colorTable
end

function Tip:setFont(newFontPath, newFontSize)
	--font type limitations, comment to avoid verification
	newFontPath = string.lower(findDigitAndReplace(newFontPath,"\\","/"))
	if split(newFontPath,'.')[table.maxn(split(newFontPath,'.'))]~='ttf' and split(newFontPath,'.')[table.maxn(split(newFontPath,'.'))]~='fon' then return end
	self.fontPath = newFontPath
	self.fontName = split(newFontPath,'/')[table.maxn(split(newFontPath,'/'))]
	--font size limitations, comment to avoid verification
	if type(newFontSize)~='number' then newFontSize=11 end
	if newFontSize<8 or newFontSize>25 then newFontSize=11--[[default]] end
	self.font = scrupp.addFont(newFontPath, newFontSize)
	self.fontSize = newFontSize
end

function Tip:getFont()
	return { name = self.fontName, size = self.fontSize }
end

function Tip:getFontPath()
	return self.fontPath
end

function Tip:toggleShowBorder()
	local show = not self.showBorder
	self.showBorder = show
	return show
end

function Tip:getBgColor()
	return { color = tostring(self.bgColor[1]).." "..tostring(self.bgColor[2]).." "..tostring(self.bgColor[3]), alpha = tostring(self.bgColor[4]) , rgb = {self.bgColor[1], self.bgColor[2], self.bgColor[3]}, a = self.bgColor[4], rgba = {self.bgColor[1], self.bgColor[2], self.bgColor[3], self.bgColor[4]} }
end

function Tip:getFgColor()
	return { color = tostring(self.fgColor[1]).." "..tostring(self.fgColor[2]).." "..tostring(self.fgColor[3]), alpha = tostring(self.fgColor[4]) , rgb = {self.fgColor[1], self.fgColor[2], self.fgColor[3]}, a = self.fgColor[4], rgba = {self.fgColor[1], self.fgColor[2], self.fgColor[3], self.fgColor[4]} }
end

function Tip:getBorderColor()
	return { color = tostring(self.borderColor[1]).." "..tostring(self.borderColor[2]).." "..tostring(self.borderColor[3]), alpha = tostring(self.borderColor[4]) , rgb = {self.borderColor[1], self.borderColor[2], self.fgColor[3]}, a = self.borderColor[4], rgba = {self.borderColor[1], self.borderColor[2], self.borderColor[3], self.borderColor[4]} }
end

function Tip:hide()
	self.isShowing = false
end

function Tip:show()
	self.isShowing = true
end

function Tip:render(x, y, text)
	--avoid mouse cursor
	x=x+16
	y=y+20
	if self.isShowing then
		--avoid screen limits
		local tw, th = self.font:getTextSize(text)
		while tw+x+4 > scrupp.getWindowWidth()-1 do x = x -1 end
		while th+y+4 > scrupp.getWindowHeight()-1 do y = y - 1 end
		--first the back
		if self.hasImage then
			local IW, IH = self.font:getTextSize(text)
			IW = IW + 8
			IH = IH + 8
			newDimension = { IW, IH }
			self.image:setScale(newDimension[1]/self.dimension[1], newDimension[2]/self.dimension[2])
			self.image:render(x-4, y-4)
		else
			--back color
			local tip_background_lines = { color = self.bgColor, antialiasing = true, connect = true, fill=true}
			local w, h = self.font:getTextSize(text)
			tip_background_lines[#tip_background_lines+1] = x-4
			tip_background_lines[#tip_background_lines+1] = y-4
			tip_background_lines[#tip_background_lines+1] = x+w+4
			tip_background_lines[#tip_background_lines+1] = y-4
			tip_background_lines[#tip_background_lines+1] = x+w+4
			tip_background_lines[#tip_background_lines+1] = y+h+4
			tip_background_lines[#tip_background_lines+1] = x-4
			tip_background_lines[#tip_background_lines+1] = y+h+4
			tip_background_lines[#tip_background_lines+1] = x-4
			tip_background_lines[#tip_background_lines+1] = y-4
			scrupp.draw(tip_background_lines)
			tip_background_lines = nil
		end
		--border...
		if self.showBorder then
			local tip_lines = { color = self.borderColor, antialiasing = true, connect = true }
			local w, h = self.font:getTextSize(text)
			tip_lines[#tip_lines+1] = x-4
			tip_lines[#tip_lines+1] = y-4
			tip_lines[#tip_lines+1] = x+w+4
			tip_lines[#tip_lines+1] = y-4
			tip_lines[#tip_lines+1] = x+w+4
			tip_lines[#tip_lines+1] = y+h+4
			tip_lines[#tip_lines+1] = x-4
			tip_lines[#tip_lines+1] = y+h+4
			tip_lines[#tip_lines+1] = x-4
			tip_lines[#tip_lines+1] = y-4
			scrupp.draw(tip_lines)
			tip_lines=nil
		end
		--and the text...
		local text = findDigitAndReplace(text, "\\", "/")
		local w, h = self.font:getTextSize(text)
		if w>0 then
			local textToDraw = self.font:generateImage{text, color = self.fgColor}
			local position = { x , y }
			local dimension = { w, h }
			textToDraw:render(position[1]+((dimension[1]/2)-(w/2)),position[2]+((dimension[2]/2)-(h/2)))
		end
	end
end

return Tip
