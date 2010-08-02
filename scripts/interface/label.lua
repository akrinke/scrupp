--[[
-- $Id$
-- Interface for Scrupp; by: Everton Costa
-- Label class
-- See Copyright Notice in COPYRIGHT
--]]

require "class"
require "interface.utils"

local split = interface.utils.split
local findDigitAndReplace = interface.utils.findDigitAndReplace

local Label = class(function(self, text, font, align)
	self.text = "Label"
	if text~=nil then
		self.text=text
	end
	self.align = "L"
	if align~=nil then
		self.align=string.upper(align)
		if self.align~="L" and  self.align~="C" and  self.align~="R" then
			self.align = "L"
		end
	end
	self.fgColor = { 0, 0, 0, 255}
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
	local w, h = self.font:getTextSize(self.text)
	self.dimension = { w, h }
	self.position = { scrupp.getWindowWidth()/2-self.dimension[1]/2 , scrupp.getWindowHeight()/2-self.dimension[2]/2}
	self.bold = false
	self.z = -1
	self.visible = true
	self.connected = false
	self.connector = nil
end)

function Label:remove()
	self.text = ""
	self.font = nil
	self.visible = false
	if self.connected then
		self.connector:removeControl(self)
		self.connected = false
		self.connector = nil
	end
	self.dimension = nil
	self.position = nil
	self = nil
end

function Label:setPosition(newPosition)
	--comment to avoid screen limitations
	if newPosition[1]>scrupp.getWindowWidth()-self.dimension[1] then newPosition[1] = scrupp.getWindowWidth()-self.dimension[1] end
	if newPosition[1]<0 then newPosition[1] = 0 end
	if newPosition[2]>scrupp.getWindowHeight()-self.dimension[2] then newPosition[2] = scrupp.getWindowHeight()-self.dimension[2] end
	if newPosition[2]<0 then newPosition[2] = 0 end
	--
	self.position = newPosition
end

function Label:getPosition()
	return { x=self.position[1], y=self.position[2] }
end

function Label:getDimension()
	return { w=self.dimension[1], h=self.dimension[2] }
end

function Label:setDimension(newDimension)
	if type(newDimension[1])~='number' or type(newDimension[2])~='number' then return end
	if newDimension[1]<=0 or newDimension[2]<=0 then return end
	self.dimension = newDimension
end

function Label:setAlign(newAlign)
	if newAlign~=nil then
		self.align=string.upper(newAlign)
		if self.align~="L" and  self.align~="C" and  self.align~="R" then
			self.align = "L"
		end
	end
end

function Label:getAlign()
	return  self.align
end

function Label:setVisible(newState)
	if type(newState)~='boolean' then newState=true end
	self.visible = newState
end

function Label:getVisible()
	return self.visible
end

function Label:setBoldState(newState)
	if type(newState)~='boolean' then newState=true end
	self.bold = newState
end

function Label:getBoldState()
	return self.bold
end

function Label:setZ(newZ)
	if newZ == nil then self.z = -1 end
	if type(newZ)~='number' then return end
	if newZ < -1 then self.z = -1 end
	self.z = newZ
end

function Label:getZ()
	return self.z
end

function Label:setFont(newFontPath, newFontSize)
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

function Label:getFont()
	return { name = self.fontName, size = self.fontSize }
end

function Label:getFontPath()
	return self.fontPath
end

function Label:setText(newText)
	self.text = tostring(newText)
end

function Label:getText()
	return self.text
end

function Label:Connect(theContainer)
	--theContainer must be a desktop
	if theContainer == nil then return end
	if self.connected == false then theContainer:addControl(self) self.connected=true self.connector=theContainer end
end

function Label:Disconnect()
	if self.connected then
		self.connector:removeControl(self)
		self.connected = false
		self.connector = nil
	end
end

function Label:setFgColor(newColor)
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

function Label:getFgColor()
	return { color = tostring(self.fgColor[1]).." "..tostring(self.fgColor[2]).." "..tostring(self.fgColor[3]), alpha = tostring(self.fgColor[4]) , rgb = {self.fgColor[1], self.fgColor[2], self.fgColor[3]}, a = self.fgColor[4], rgba = {self.fgColor[1], self.fgColor[2], self.fgColor[3], self.fgColor[4]} }
end

function Label:updateMousePressed(mouse)
	--pass
end

function Label:updateMouseReleased(mouse)
	--pass
end

function Label:updateKeyPressed(key)
	--pass
end

function Label:updateKeyReleased(key)
	--pass
end

function Label:render()
	if self.visible then
		if self.bold then
			self.text = findDigitAndReplace(self.text, "\\", "/")
			local w, h = self.font:getTextSize(self.text)
			if w > 0 then
				local textToDraw = self.font:generateImage{self.text, color = self.fgColor}
				if self.align == "R" then
					textToDraw:render(self.position[1]+self.dimension[1],self.position[2]+((self.dimension[2]/2)-(h/2)))
					textToDraw:render(self.position[1]+self.dimension[1]+1,self.position[2]+((self.dimension[2]/2)-(h/2)))
				elseif self.align == "L" then
					textToDraw:render(self.position[1],self.position[2]+((self.dimension[2]/2)-(h/2)))
					textToDraw:render(self.position[1]+1,self.position[2]+((self.dimension[2]/2)-(h/2)))
				else
					textToDraw:render(self.position[1]+((self.dimension[1]/2)-(w/2)),self.position[2]+((self.dimension[2]/2)-(h/2)))
					textToDraw:render(self.position[1]+((self.dimension[1]/2)-(w/2))+1,self.position[2]+((self.dimension[2]/2)-(h/2)))
				end
			end
		else
			self.text = findDigitAndReplace(self.text, "\\", "/")
			local w, h = self.font:getTextSize(self.text)
			if w > 0 then
				local textToDraw = self.font:generateImage{self.text, color = self.fgColor}
				if self.align == "R" then
					textToDraw:render(self.position[1]+self.dimension[1],self.position[2]+((self.dimension[2]/2)-(h/2)))
				elseif self.align == "L" then
					textToDraw:render(self.position[1],self.position[2]+((self.dimension[2]/2)-(h/2)))
				else
					textToDraw:render(self.position[1]+((self.dimension[1]/2)-(w/2)),self.position[2]+((self.dimension[2]/2)-(h/2)))
				end
			end
		end
	end
end

return Label
