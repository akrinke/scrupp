--[[
-- $Id$
-- Interface for Scrupp; by: Everton Costa
-- Desktop class
-- See Copyright Notice in COPYRIGHT
--]]

require "class"

local Desktop = class(function(self)
	self.controls = {}
	self.hasImage = false
	self.imagePath = ""
	self.image = nil
	self.alpha = 255
	self.visible = true
	self.bgColor = { 0, 0, 0, 255}
	self.showBack = false
	self.tipToRender = {nil, -1, -1, "" }
end)

function Desktop:setTipToRender(theTip, x, y, text)
	if theTip==nil then self.tipToRender = {nil, -1, -1, "" } return end
	self.tipToRender = { theTip, x, y, text }
end

function Desktop:setBgColor(newColor)
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
	self.showBack = true
	self.bgColor = colorTable
end

function Desktop:remove()
	self.controls = nil
	if self.hasImage then
		self.imagePath = ""
		self.image = nil
	end
	self = nil
end

function Desktop:toggleShowBgColor()
	local show = not self.showBack
	self.showBack = show
	return show
end

function Desktop:getBgColor()
	return { color = tostring(self.bgColor[1]).." "..tostring(self.bgColor[2]).." "..tostring(self.bgColor[3]), alpha = tostring(self.bgColor[4]) , rgb = {self.bgColor[1], self.bgColor[2], self.bgColor[3]}, a = self.bgColor[4], rgba = {self.bgColor[1], self.bgColor[2], self.bgColor[3], self.bgColor[4]} }
end

function Desktop:hasControl(theControl)
	for i=1,#self.controls do
		if self.controls[i]==theControl then return true end
	end
	return false
end

function Desktop:addControl(theControl)
	if theControl==nil then return end
	for i=1,#self.controls do
		if self.controls[i]==theControl then return end
	end
	self.controls[#self.controls+1] = theControl
end

function Desktop:removeControl(theControl)
	if theControl==nil then return end
	for i=1,#self.controls do
		if self.controls[i]==theControl then table.remove(self.controls, i) return end
	end
end

function Desktop:setImage(newImage)
	if newImage == nil then self.hasImage = false self.image = nil self.imagePath = "" return end
	self.image = scrupp.addImage(newImage)
	dimension = { self.image:getWidth(), self.image:getHeight() }
	self.hasImage = true
	self.imagePath = newImage
	newDimension = { scrupp.getWindowWidth(), scrupp.getWindowHeight() }
	self.image:setScale((newDimension[1]*self.image:getScaleX())/dimension[1], (newDimension[2]*self.image:getScaleY())/dimension[2])
end

function Desktop:getImage()
	return self.imagePath
end

function Desktop:setAlpha(newAlpha)
	if type(newAlpha)~='number' then return end
	if newAlpha >= 0 and newAlpha <= 255 then
		self.alpha = newAlpha
	else
		self.alpha = 255 --default
	end
end

function Desktop:getAlpha()
	return self.alpha
end

function Desktop:setVisible(newState)
	if type(newState)~='boolean' then newState=true end
	self.visible = newState
end

function Desktop:getVisible()
	return self.visible
end

function Desktop:update()
	if self.visible then
		if self.showBack then
			local dkt_lines = { color = self.bgColor, antialiasing = false,connect = true, fill=true}
			dkt_lines[#dkt_lines+1] = 0
			dkt_lines[#dkt_lines+1] = 0
			dkt_lines[#dkt_lines+1] = scrupp.getWindowWidth()
			dkt_lines[#dkt_lines+1] = 0
			dkt_lines[#dkt_lines+1] = scrupp.getWindowWidth()
			dkt_lines[#dkt_lines+1] = scrupp.getWindowHeight()
			dkt_lines[#dkt_lines+1] = 0
			dkt_lines[#dkt_lines+1] = scrupp.getWindowHeight()
			dkt_lines[#dkt_lines+1] = 0
			dkt_lines[#dkt_lines+1] = 0
			scrupp.draw(dkt_lines)
			dkt_lines=nil
		end
		if self.hasImage then
			self.image:setAlpha(self.alpha)
			self.image:render(0,0)-- z = -2
		end
		if #self.controls==0 then return end
		local zt = {}
		for i=1,#self.controls do
			local currentZ = self.controls[i]:getZ()
			if currentZ ~= -1 then
				zt[currentZ+#zt+1] = self.controls[i]
			end
		end
		for i=1,#self.controls do
			local currentZ = self.controls[i]:getZ()
			if currentZ == -1 then
				--table end
				zt[#zt+1] = self.controls[i]
			end
		end
		--then render all control, except tipToRender!
		for i=1, #zt do
			if zt[i]==nil then
				zt[i]=nil--pass
			else
				zt[i]:render()
			end
		end
		--tip to render code
		if self.tipToRender[1]~=nil then
			self.tipToRender[1]:render(self.tipToRender[2], self.tipToRender[3], self.tipToRender[4])
		end
	end
end

function Desktop:updateMousePressed(mouse)
	if self.visible then
		for i=1,#self.controls do
			if self.controls[i]~=nil then
				self.controls[i]:updateMousePressed(mouse)
			end
		end
	end
end

function Desktop:updateMouseReleased(mouse)
	if self.visible then
		for i=1,#self.controls do
			if self.controls[i]~=nil then
				self.controls[i]:updateMouseReleased(mouse)
			end
		end
	end
end

function Desktop:updateKeyPressed(key)
	if self.visible then
		for i=1,#self.controls do
			if self.controls[i]~=nil then
				self.controls[i]:updateKeyPressed(key)
			end
		end
	end
end

function Desktop:updateKeyReleased(key)
	if self.visible then
		for i=1,#self.controls do
			if self.controls[i]~=nil then
				self.controls[i]:updateKeyReleased(key)
			end
		end
	end
end

return Desktop
