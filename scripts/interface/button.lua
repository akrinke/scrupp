--[[
-- $Id$
-- Interface for Scrupp; by: Everton Costa
-- Button class
-- See Copyright Notice in COPYRIGHT
--]]

require "class"
require "interface.utils"

local split = interface.utils.split
local findDigitAndReplace = interface.utils.findDigitAndReplace

local theme = require "interface.theme_default"
local styles = {"normal", "disabled", "pressed", "focused"}

local Button = class(function(self, onAction, images, font, onMouseOver, onMouseLeave)
	local width, height = 100, 40
	if type(images)=='nil' then
		self.images = { 
			theme.createButtonImage(width, height, "normal"),
			theme.createButtonImage(width, height, "disabled"),
			theme.createButtonImage(width, height, "pressed"),
			theme.createButtonImage(width, height, "focused")
		}
	else
		if images[2]==nil then images[2] = images[1] end
		if images[3]==nil then images[3] = images[1] end
		if images[4]==nil then images[4] = images[1] end
		images[1] = scrupp.addImage(images[1])
		images[2] = scrupp.addImage(images[2])
		images[3] = scrupp.addImage(images[3])
		images[4] = scrupp.addImage(images[4])
		self.images = {images[1], images[2], images[3], images[4]}
	end
	self.dimension = {self.images[1]:getWidth() , self.images[1]:getHeight() }
	self.position = { scrupp.getWindowWidth()/2-self.dimension[1]/2 , scrupp.getWindowHeight()/2-self.dimension[2]/2}
	self.keys = {}
	self.enabled = true
	if type(font)=='nil' then
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
	self.clicked = false
	self.text = "Button"
	self.fgColor = { 0, 0, 0, 255}
	self.callback = onAction
	self.callbackMouseOver = onMouseOver
	self.callbackMouseLeave = onMouseLeave
	self.mouseOver = false
	self.mouseLeave = true
	self.numberpressed = 0
	self.visible = true
	self.alpha = 255
	self.z = -1
	self.connected = false
	self.connector = nil
	self.tip = nil
	self.tipText = ""
	self.tipDelay = 250
	self.tipTime = 0
end)

function Button:removeTip()
	self.tip = nil
	self.tipText = ""
	self.tipDelay = 250
	self.tipTime = 0
end

function Button:getTip()
	if self.tip==nil then return end
	return { tip = self.tip, text = self.tipText, delay = self.tipDelay }
end

function Button:Tip(theTip, text, delay)
	if self.tip~=nil then
		if theTip==self.tip then return end
	end
	if theTip~=nil then self.tip = theTip end
	self.tipText = tostring(text)
	if type(delay)=="number" then self.tipDelay = delay end
	self.tipTime = 0
end

function Button:setDimension(newDimension)
	if type(newDimension[1])~='number' or type(newDimension[2])~='number' then return end
	if newDimension[1]<=0 or newDimension[2]<=0 then return end
	for i=1,4 do
		self.images[i] = theme.createButtonImage(newDimension[1], newDimension[2], styles[i])
		--self.images[i]:setScale((newDimension[1]*self.images[i]:getScaleX())/self.dimension[1], (newDimension[2]*self.images[i]:getScaleY())/self.dimension[2])
	end
	self.dimension = newDimension
end

function Button:setZ(newZ)
	if newZ == nil then self.z = -1 end
	if type(newZ)~='number' then return end
	if newZ < -1 then self.z = -1 end
	self.z = newZ
end

function Button:getZ()
	return self.z
end

function Button:Connect(theContainer)
	--theContainer must be a desktop
	if theContainer == nil then return end
	if self.connected == false then theContainer:addControl(self) self.connected=true self.connector=theContainer end
end

function Button:Disconnect()
	if self.connected then
		self.connector:removeControl(self)
		self.connected = false
		self.connector = nil
	end
end

function Button:getAlpha()
	return self.alpha
end

function Button:remove()
	self.visible = false
	self.enabled = false
	self.images = nil
	self.font = nil
	self.callback = nil
	self.callbackMouseOver = nil
	self.callbackMouseLeave = nil
	self.fontName = ""
	self.fontPath = ""
	self.fontSize = ""
	self.keys = nil
	self.tip = nil
	self.tipText = ""
	if self.connected then
		self.connector:removeControl(self)
		self.connected = false
		self.connector = nil
	end
	self.dimension = nil
	self.position = nil
	self.fgColor = nil
	self = nil
end

function Button:setAlpha(newAlpha)
	if type(newAlpha)~='number' then return end
	if newAlpha >= 0 and newAlpha <= 255 then
		self.alpha = newAlpha
	else
		self.alpha = 255 --default
	end
end

function Button:setImages(newImages)
	if newImages[1] == nil then return end
	if newImages[2]==nil then newImages[2] = newImages[1] end
	if newImages[3]==nil then newImages[3] = newImages[1] end
	if newImages[4]==nil then newImages[4] = newImages[1] end
	self.images = {newImages[1],newImages[2],newImages[3],newImages[4]}
	self.dimension = { self.images[1]:getWidth(), self.images[1]:getHeight() }
end

function Button:getImages()
	return {normal = self.images[1], disabled = self.images[2], pressed = self.images[3], focused = self.images[4]}
end

function Button:setKeys(newKeys)
	if newKeys[1] == nil then return end
	self.keys = newKeys
end

function Button:setVisible(newState)
	if type(newState)~='boolean' then newState=true end
	self.visible = newState
end

function Button:getVisible()
	return self.visible
end

function Button:getKeys()
	return self.keys
end

function Button:setPosition(newPosition)
	--comment to avoid screen limitations
	if newPosition[1]>scrupp.getWindowWidth()-self.dimension[1] then newPosition[1] = scrupp.getWindowWidth()-self.dimension[1] end
	if newPosition[1]<0 then newPosition[1] = 0 end
	if newPosition[2]>scrupp.getWindowHeight()-self.dimension[2] then newPosition[2] = scrupp.getWindowHeight()-self.dimension[2] end
	if newPosition[2]<0 then newPosition[2] = 0 end
	--
	self.position = newPosition
end

function Button:getPosition()
	return { x=self.position[1], y=self.position[2] }
end

function Button:getDimension()
	return { w=self.dimension[1], h=self.dimension[2] }
end

function Button:setEnabled(newState)
	if type(newState)~='boolean' then newState=true end
	self.enabled = newState
end

function Button:getEnabled()
	return self.enabled
end

function Button:setText(newText)
	self.text = tostring(newText)
end

function Button:getText()
	return self.text
end

function Button:getFont()
	return { name = self.fontName, size = self.fontSize }
end

function Button:getFontPath()
	return self.fontPath
end

function Button:setFont(newFontPath, newFontSize)
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

function Button:setFgColor(newColor)
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

function Button:getFgColor()
	return { color = tostring(self.fgColor[1]).." "..tostring(self.fgColor[2]).." "..tostring(self.fgColor[3]), alpha = tostring(self.fgColor[4]) , rgb = {self.fgColor[1], self.fgColor[2], self.fgColor[3]}, a = self.fgColor[4], rgba = {self.fgColor[1], self.fgColor[2], self.fgColor[3], self.fgColor[4]} }
end

function Button:updateMousePressed(mouse)
	if self.enabled then
		--is clicked?
		if mouse=='left' and scrupp.getMouseX()>=self.position[1] and scrupp.getMouseX()<=self.position[1]+self.dimension[1] and scrupp.getMouseY()>=self.position[2] and scrupp.getMouseY()<=self.position[2]+self.dimension[2] then
			self.numberpressed=0
			self.clicked = true
		else
			self.numberpressed=0
			self.clicked = false
		end
	else
		self.numberpressed=0
		self.clicked = false
	end
end

function Button:updateMouseReleased(mouse)
	if self.enabled then
		--is released?
		if  mouse=='left' and scrupp.getMouseX()>=self.position[1] and scrupp.getMouseX()<=self.position[1]+self.dimension[1] and scrupp.getMouseY()>=self.position[2] and scrupp.getMouseY()<=self.position[2]+self.dimension[2] then
			if self.clicked then
				--call callback
				if self.visible then
					self.callback()
					self.numberpressed=0
					self.clicked = false
				end
			end
		else
			self.numberpressed=0
			self.clicked = false
		end
	else
		self.numberpressed=0
		self.clicked = false
	end
end

function Button:updateKeyPressed(key)
	if #self.keys == 0 then return end
	key = string.lower(key)
	for i=1, #self.keys do
		if key == string.lower(self.keys[i]) then
			self.numberpressed = self.numberpressed + 1
		end
	end
	if self.numberpressed == #self.keys then
		--call callback
		if self.visible then
			self.callback()
			self.clicked = false
		end
	end
end

function Button:updateKeyReleased(key)
	if #self.keys == 0 then return end
	key = string.lower(key)
	for i=1, #self.keys do
		if key == string.lower(self.keys[i]) then
			self.numberpressed = self.numberpressed - 1
		end
	end
end

function Button:render()
	local imageIndexToDraw = 1 -- default
	if self.enabled == false then imageIndexToDraw = 2 end
	if self.enabled then
		if self.clicked then imageIndexToDraw = 3 end
		if self.clicked == false and scrupp.getMouseX()>=self.position[1] and scrupp.getMouseX()<=self.position[1]+self.dimension[1] and scrupp.getMouseY()>=self.position[2] and scrupp.getMouseY()<=self.position[2]+self.dimension[2] then
			imageIndexToDraw=4
		end
	end
	if self.visible then
		--first the button
		self.images[imageIndexToDraw]:setAlpha(self.alpha)
		self.images[imageIndexToDraw]:render(self.position[1], self.position[2])
		--now the text
		local textToDraw = self.font:generateImage{self.text, color = self.fgColor}
		local w, h = self.font:getTextSize(self.text)
		textToDraw:render(self.position[1]+((self.dimension[1]/2)-(w/2)),self.position[2]+((self.dimension[2]/2)-(h/2)))
		if scrupp.getMouseX()>=self.position[1] and scrupp.getMouseX()<=self.position[1]+self.dimension[1] and scrupp.getMouseY()>=self.position[2] and scrupp.getMouseY()<=self.position[2]+self.dimension[2] then
			--Mouse Over call callback
			if self.mouseOver==false and self.callbackMouseOver~=nil and self.enabled then self.callbackMouseOver(self) self.mouseOver=true self.mouseLeave=false end
			--Tip
			if self.tip~=nil then
				self.tipTime = self.tipTime + 1
				if self.tipTime > self.tipDelay then
					self.tip:show()
					if 	self.connected then
						self.connector:setTipToRender(self.tip, scrupp.getMouseX(), scrupp.getMouseY(), self.tipText)
					else
						--Warning: Z order will go bad
						self.tip:render(scrupp.getMouseX(), scrupp.getMouseY(), self.tipText)
					end
				end
			end
		else
			--Mouse Leave call callback
			if self.mouseLeave==false and self.callbackMouseOver~=nil and self.enabled then self.callbackMouseLeave(self) self.mouseOver=false self.mouseLeave=true end
			--Tip
			if self.tip~=nil and self.tipTime>0 then
				self.tip:hide()
				self.tipTime = 0
				if self.connected then self.connector:setTipToRender() end
			end
		end
	end
end

return Button
