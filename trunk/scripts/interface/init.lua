--[[
-- $Id$
-- Interface for Scrupp; by: Everton Costa
-- See Copyright Notice in COPYRIGHT
--]]

print("Uses Interface for Scrupp; by: Everton Costa")
print("Uses Class Library")

local theme = require "interface.theme_default"
local styles = {"normal", "disabled", "pressed", "focused"}

require "class"

--Global functions
function split(v,s)
	local r={}
	local t=""
	for i=1,#v do
		if string.sub(v,i,i)==s then
			table.insert(r, t)
			t=""
		else
			t=t..string.sub(v,i,i)
		end
	end
	table.insert(r, t)
	return r
end
function findDigitAndReplace(v,f,r)
	local e = ""
	local t = ""
	for i=1,#v do
		if string.lower(string.sub(v,i,i+#f-1))==string.lower(f) then
			t = r
		else
			t = string.sub(v,i,i)
		end
		e = e..t
	end
	return e
end
function insert(v,p,c)
	local a = {}
	local r = ""
	if c=="" then return v end
	if p>#v+1 or p<1 then return v end
	for i=1,#v do
		a[i] = string.sub(v,i,i)
	end
	for i=1,p-1 do
		r = r..a[i]
	end
	r = r..c
	for i=p,#v do
		r = r..a[i]
	end
	return r
end
function remove(v,p,n)
	v = tostring(v)
	if n==nil then n=0 end
	if p==nil then p=#v end
	if p>#v+1 or p<1 then return v end
	if p+n>#v+1 or p+n<0 then return "" end
	if n==0 then n=-1 end
	local a = {}
	local r = ""
	for i=1,#v do
		a[i] = string.sub(v,i,i)
	end
	if n>0 then
		for i=1,p-1 do r = r..a[i] end
	else
		for i=1,p-1+n do r = r..a[i] end
	end
	if n>0 then
		for i=p+n,#v do r = r..a[i] end
	else
		for i=p,#v do r = r..a[i] end
	end
	return r
end
function findChar(v, c)
	for i=1, #v do
		if string.lower(string.sub(v,i,i))==string.lower(tostring(c)) then return true end
	end
	return false
end
--Interface Table Declaration
interface = {
--Field
addField = class(function(self, text, font, images, onKeyEnter, onChange)
	self.text = "Field"
	if text~=nil then
		self.text=text
	end
	self.fgColor = { 0, 0, 0, 255}
	self.bgColor = { 255, 255, 255, 255}
	self.borderColor = { 0, 0, 0, 255}
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
	self.dimension = { 80 , 20 }
	self.position = { scrupp.getWindowWidth()/2-self.dimension[1]/2 , scrupp.getWindowHeight()/2-self.dimension[2]/2}
	self.images = {nil, nil, nil }
	if images~=nil then
		if images[2]==nil then images[2] = images[1] end
		if images[3]==nil then images[3] = images[1] end
		images[1] = scrupp.addImage(images[1])
		images[2] = scrupp.addImage(images[2])
		images[3] = scrupp.addImage(images[3])
		self.images = {images[1],images[2],images[3]}
		for i=1, #self.images do
			if self.images[i]~= nil then
				self.images[i]:setScale(self.images[i]:getWidth()/self.dimension[1], self.images[i]:getHeight()/self.dimension[2])
			end
		end
	end
	self.z = -1
	self.visible = true
	self.enabled = true
	self.focused = false
	self.callbackOnKeyEnter = onKeyEnter
	self.callbackOnKeyChange = onChange
	self.connected = false
	self.connector = nil
	self.tip = nil
	self.alpha = 255
	self.tipText = ""
	self.tipDelay = 250
	self.tipTime = 0
	self.vtText = ""
	self.vtPosition = 1
	self.shift = false
	self.capslock = false
	self.keyboard = "PS/2"
	self.pressedKey = ""
	self.lastkey = ""
	self.mask = "ASCII"
	self.length = -1
	self.tick = true
	self.tickTime = 50
end),
--Label
addLabel = class(function(self, text, font, align)
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
end),
--Tip
addTip = class(function(self, image, font)
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
end),
--desktop
addDesktop = class(function(self)
	self.controls = {}
	self.hasImage = false
	self.imagePath = ""
	self.image = nil
	self.alpha = 255
	self.visible = true
	self.bgColor = { 0, 0, 0, 255}
	self.showBack = false
	self.tipToRender = {nil, -1, -1, "" }
end),
--button
addButton = class(function(self, onAction, images, font, onMouseOver, onMouseLeave)
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
}
--Class Declaration
interface_addField = interface.addField
interface_addLabel = interface.addLabel
interface_addTip = interface.addTip
interface_addDesktop = interface.addDesktop
interface_addButton = interface.addButton
--==================================================================================================================
--Begin of addField
--==================================================================================================================
function interface_addField:setLength(newLength)
	if tostring(newLength)=="" or tostring(newLength)=="*" then newLength  = -1 end
	if type(newLength) ~= "number" then return end
	newLength = math.abs(newLength)
	if newLength < 0 then
		self.length = -1
	else
		self.length = newLength
	end
end
function interface_addField:getLength()
	return self.length
end
function interface_addField:getValue()
	if self.mask == "ASCII" or self.mask == "UTF-8" then
		return tostring(self.text)
	else
		return tonumber(self.text)
	end
end
function interface_addField:setMask(newMask)
	if newMask == nil then return end
	newMask = string.lower(newMask)
	if newMask == "ascii" then
		self.mask = "ASCII"
	elseif newMask == "utf-8" then
		self.mask = "UTF-8"
	elseif newMask == "integer" then
		self.mask = "integer"
	elseif newMask == "decimal" then
		self.mask = "decimal"
	end
end
function interface_addField:getMask()
	return self.mask
end
function interface_addField:setKeyboard(newKeyboardType)
	if tostring(newKeyboardType)=="1" then newKeyboardType = "PS/2"
	elseif tostring(newKeyboardType)=="2" then newKeyboardType = "scrupp" end
	if string.upper(newKeyboardType)=="PS/2" or string.lower(newKeyboardType)=="scrupp" then
		self.keyboard = newKeyboardType
	end
end
function interface_addField:getKeyboard()
	return self.keyboard
end
function interface_addField:removeTip()
	self.tip = nil
	self.tipText = ""
	self.tipDelay = 250
	self.tipTime = 0
end
function interface_addField:getTip()
	if self.tip==nil then return end
	return { tip = self.tip, text = self.tipText, delay = self.tipDelay }
end
function interface_addField:Tip(theTip, text, delay)
	if self.tip~=nil then
		if theTip==self.tip then return end
	end
	if theTip~=nil then self.tip = theTip end
	self.tipText = tostring(text)
	if type(delay)=="number" then self.tipDelay = delay end
	self.tipTime = 0
end
function interface_addField:setDimension(newDimension)
	if type(newDimension[1])~='number' or type(newDimension[2])~='number' then return end
	if newDimension[1]<=0 or newDimension[2]<=0 then return end
	if self.images[1]~=nil then
		for i=1,#self.images do
			if self.images[i]~=nil then
				self.images[i]:setScale((newDimension[1]*self.images[i]:getScaleX())/self.dimension[1], (newDimension[2]*self.images[i]:getScaleY())/self.dimension[2])
			end
		end
	end
	self.dimension = newDimension
end
function interface_addField:setZ(newZ)
	if newZ == nil then self.z = -1 end
	if type(newZ)~='number' then return end
	if newZ < -1 then self.z = -1 end
	self.z = newZ
end
function interface_addField:getZ()
	return self.z
end
function interface_addField:Connect(theContainer)
	--theContainer must be a desktop
	if theContainer == nil then return end
	if self.connected == false then theContainer:addControl(self) self.connected=true self.connector=theContainer end
end
function interface_addField:Disconnect()
	if self.connected then
		self.connector:removeControl(self)
		self.connected = false
		self.connector = nil
	end
end
function interface_addField:getAlpha()
	return self.alpha
end
function interface_addField:remove()
	self.visible = false
	self.enabled = false
	self.images = nil
	self.font = nil
	self.callbackOnKeyEnter = nil
	self.callbackOnKeyChange = nil
	self.fontName = ""
	self.fontPath = ""
	self.fontSize = ""
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
	self.bgColor = nil
	self.borderColor = nil
	self = nil
end
function interface_addField:setAlpha(newAlpha)
	if type(newAlpha)~='number' then return end
	if newAlpha >= 0 and newAlpha <= 255 then
		self.alpha = newAlpha
	else
		self.alpha = 255 --default
	end
end
function interface_addField:setImages(newImages)
	if newImages[1] == nil then return end
	if newImages[2]==nil then newImages[2] = newImages[1] end
	if newImages[3]==nil then newImages[3] = newImages[1] end
	self.images = {newImages[1],newImages[2],newImages[3]}
	self.dimension = { self.images[1]:getWidth(), self.images[1]:getHeight() }
end
function interface_addField:getImages()
	return {normal = self.images[1], disabled = self.images[2], focused = self.images[3]}
end
function interface_addField:setVisible(newState)
	if type(newState)~='boolean' then newState=true end
	self.visible = newState
end
function interface_addField:getVisible()
	return self.visible
end
function interface_addField:setPosition(newPosition)
	--comment to avoid screen limitations
	if newPosition[1]>scrupp.getWindowWidth()-self.dimension[1] then newPosition[1] = scrupp.getWindowWidth()-self.dimension[1] end
	if newPosition[1]<0 then newPosition[1] = 0 end
	if newPosition[2]>scrupp.getWindowHeight()-self.dimension[2] then newPosition[2] = scrupp.getWindowHeight()-self.dimension[2] end
	if newPosition[2]<0 then newPosition[2] = 0 end
	--
	self.position = newPosition
end
function interface_addField:getPosition()
	return { x=self.position[1], y=self.position[2] }
end
function interface_addField:getDimension()
	return { w=self.dimension[1], h=self.dimension[2] }
end
function interface_addField:setEnabled(newState)
	if type(newState)~='boolean' then newState=true end
	self.enabled = newState
end
function interface_addField:getEnabled()
	return self.enabled
end
function interface_addField:setText(newText)
	self.text = tostring(newText)
end
function interface_addField:getText()
	return self.text
end
function interface_addField:getFont()
	return { name = self.fontName, size = self.fontSize }
end
function interface_addField:getFontPath()
	return self.fontPath
end
function interface_addField:setFont(newFontPath, newFontSize)
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
function interface_addField:setFgColor(newColor)
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
function interface_addField:getFgColor()
	return { color = tostring(self.fgColor[1]).." "..tostring(self.fgColor[2]).." "..tostring(self.fgColor[3]), alpha = tostring(self.fgColor[4]) , rgb = {self.fgColor[1], self.fgColor[2], self.fgColor[3]}, a = self.fgColor[4], rgba = {self.fgColor[1], self.fgColor[2], self.fgColor[3], self.fgColor[4]} }
end
function interface_addField:setBgColor(newColor)
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
function interface_addField:getBgColor()
	return { color = tostring(self.bgColor[1]).." "..tostring(self.bgColor[2]).." "..tostring(self.bgColor[3]), alpha = tostring(self.bgColor[4]) , rgb = {self.bgColor[1], self.bgColor[2], self.bgColor[3]}, a = self.bgColor[4], rgba = {self.bgColor[1], self.bgColor[2], self.bgColor[3], self.bgColor[4]} }
end
function interface_addField:setBorderColor(newColor)
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
function interface_addField:getBorderColor()
	return { color = tostring(self.borderColor[1]).." "..tostring(self.borderColor[2]).." "..tostring(self.borderColor[3]), alpha = tostring(self.borderColor[4]) , rgb = {self.borderColor[1], self.borderColor[2], self.borderColor[3]}, a = self.borderColor[4], rgba = {self.borderColor[1], self.borderColor[2], self.borderColor[3], self.borderColor[4]} }
end
function interface_addField:updateMousePressed(mouse)
	if self.enabled then
		--was clicked?
		if mouse=='left' and scrupp.getMouseX()>=self.position[1] and scrupp.getMouseX()<=self.position[1]+self.dimension[1] and scrupp.getMouseY()>=self.position[2] and scrupp.getMouseY()<=self.position[2]+self.dimension[2] then
			self.focused = true
			self.text = tostring(self.text)
			self.vtPosition = #self.text+1
		else
			self.focused = false
		end
	else
		self.focused = false
	end
end
function interface_addField:updateMouseReleased(mouse)
	--pass
end
function interface_addField:updateKeyPressed(key)
	self.text = tostring(self.text)
	self.pressedKey = key
	if self.enabled and self.visible and self.focused then
		if key=="RETURN" or key=="KP_ENTER" then
			if self.mask == "decimal" and #self.text > 1 then
				if string.sub(self.text,#self.text,#self.text)=="." then
					self.text = string.sub(self.text,1,#self.text-1)
				end
			end
			if self.callbackOnKeyEnter ~= nil then
				self.callbackOnKeyEnter(self.text)
			end self.focused = false
			return
		elseif	key == "CAPSLOCK"	then self.capslock = not self.capslock
		elseif	key == "LSHIFT"		then self.shift = true
		elseif	key == "RSHIFT"		then self.shift = true
		end
		if		self.vtPosition > #self.text+1	then self.vtPosition = #self.text + 1
		elseif	self.vtPosition < 1				then self.vtPosition = 1
		end
		--global keys (shift and not shift) and conversions
		if key == "KP_PERIOD" then self.pressedKey = ","
		elseif key == "KP_PLUS" then self.pressedKey = "+"
		elseif key == "KP_MINUS" then self.pressedKey = "-"
		elseif key == "KP_MULTIPLY" then self.pressedKey = "*"
		elseif key == "KP_DIVIDE" then self.pressedKey = "/"
		elseif key == "KP9" then self.pressedKey = "9"
		elseif key == "KP8" then self.pressedKey = "8"
		elseif key == "KP7" then self.pressedKey = "7"
		elseif key == "KP6" then self.pressedKey = "6"
		elseif key == "KP5" then self.pressedKey = "5"
		elseif key == "KP4" then self.pressedKey = "4"
		elseif key == "KP3" then self.pressedKey = "3"
		elseif key == "KP2" then self.pressedKey = "2"
		elseif key == "KP1" then self.pressedKey = "1"
		elseif key == "KP0" then self.pressedKey = "0"
		elseif key == "SPACE" then self.pressedKey = " "
		elseif key == "TAB" then self.pressedKey = "	"
		end
		if self.keyboard == "PS/2" then
			if self.shift then
				if 		key == "/" then self.pressedKey = ":"
				elseif	key == "UNKNOWN" then self.pressedKey = "?"
				elseif	key == "'" then self.pressedKey = "^"
				elseif	key == "\\" then self.pressedKey = "}"
				elseif	key == "[" then self.pressedKey = "`"
				elseif	key == "]" then self.pressedKey = "{"
				elseif	key == "`" then self.pressedKey = '"'
				elseif	key == "," then self.pressedKey = "<"
				elseif	key == "." then self.pressedKey = ">"
				elseif	key == "=" then self.pressedKey = "+"
				elseif	key == "-" then self.pressedKey = "_"
				elseif	key == "0" then self.pressedKey = ")"
				elseif	key == "9" then self.pressedKey = "("
				elseif	key == "8" then self.pressedKey = "*"
				elseif	key == "7" then self.pressedKey = "&"
				elseif	key == "6" then self.pressedKey = "¨"
				elseif	key == "5" then self.pressedKey = "%"
				elseif	key == "4" then self.pressedKey = "$"
				elseif	key == "3" then self.pressedKey = "#"
				elseif	key == "2" then self.pressedKey = "@"
				elseif	key == "1" then self.pressedKey = "!"
				elseif	key == "<" then self.pressedKey = "|"
				end
			else
				if 		key == "/" then self.pressedKey = ";"
				elseif	key == "UNKNOWN" then self.pressedKey = "/"
				elseif	key == "'" then self.pressedKey = "~"
				elseif	key == "\\" then self.pressedKey = "]"
				elseif	key == "[" then self.pressedKey = "`"
				elseif	key == "]" then self.pressedKey = "["
				elseif	key == "`" then self.pressedKey = "'"
				elseif	key == "<" then self.pressedKey = "\\"
				elseif key == ";" then self.pressedKey = "ç"
				end
			end
		end
		local jumpLastKey = false
		if #self.pressedKey == 1 then
			local isUpper = nil
			if	self.shift and self.capslock then
				isUpper = false
			elseif	not self.shift and not self.capslock then
				isUpper = false
			elseif self.shift or not self.capslock then
				isUpper = true
				self.pressedKey = string.lower(self.pressedKey)
			elseif not self.shift and self.capslock then
				isUpper = true
				self.pressedKey = string.lower(self.pressedKey)
			end
			local chars = {}
			local n = 0
			chars = { "´", "`", "~", "¨", "^" }
			if self.pressedKey ~= " " then
				n = 0
				for i=1, #chars do
					if self.pressedKey==chars[i] then n = 1 end
				end
				if n == 1 then
					self.lastkey = self.pressedKey
					jumpLastKey = true
					self.pressedKey = ""
				end
			end
			if		self.lastkey == "´" and self.pressedKey == "a" then self.pressedKey="á"
			elseif self.lastkey == "`" and self.pressedKey == "a" then self.pressedKey="à"
			elseif self.lastkey == "~" and self.pressedKey == "a" then self.pressedKey="ã"
			elseif self.lastkey == "¨" and self.pressedKey == "a" then self.pressedKey="ä"
			elseif self.lastkey == "^" and self.pressedKey == "a" then self.pressedKey="â"
			elseif self.lastkey == "´" and self.pressedKey == "o" then self.pressedKey="ó"
			elseif self.lastkey == "`" and self.pressedKey == "o" then self.pressedKey="ò"
			elseif self.lastkey == "~" and self.pressedKey == "o" then self.pressedKey="õ"
			elseif self.lastkey == "¨" and self.pressedKey == "o" then self.pressedKey="ö"
			elseif self.lastkey == "^" and self.pressedKey == "o" then self.pressedKey="ô"
			elseif self.lastkey == "´" and self.pressedKey == "e" then self.pressedKey="é"
			elseif self.lastkey == "`" and self.pressedKey == "e" then self.pressedKey="è"
			elseif self.lastkey == "¨" and self.pressedKey == "e" then self.pressedKey="ë"
			elseif self.lastkey == "^" and self.pressedKey == "e" then self.pressedKey="ê"
			elseif self.lastkey == "´" and self.pressedKey == "i" then self.pressedKey="í"
			elseif self.lastkey == "`" and self.pressedKey == "i" then self.pressedKey="ì"
			elseif self.lastkey == "¨" and self.pressedKey == "i" then self.pressedKey="ï"
			elseif self.lastkey == "^" and self.pressedKey == "i" then self.pressedKey="î"
			elseif self.lastkey == "´" and self.pressedKey == "u" then self.pressedKey="ú"
			elseif self.lastkey == "`" and self.pressedKey == "u" then self.pressedKey="ù"
			elseif self.lastkey == "¨" and self.pressedKey == "u" then self.pressedKey="ü"
			elseif self.lastkey == "^" and self.pressedKey == "u" then self.pressedKey="û"
			--others digits
			elseif self.lastkey == "~" and self.pressedKey == "n" then self.pressedKey="ñ"
			elseif self.lastkey == "¨" and self.pressedKey == "y" then self.pressedKey="ÿ"
			end
			if isUpper ~= nil then
				if isUpper then
					self.pressedKey = string.upper(self.pressedKey)
				end
			end
			chars = {}
			n = 0
			--mask configurations
			if self.mask == "ASCII" then
				chars =	{
							"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z",
							"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z",
							"0","1","2","3","4","5","6","7","8","9",
							"!","\"","#","$","%","&","'","(",")","*","+",",","-",".","/",":",";","<","=",">","?","@","[","\\","]","^","_","`","{","|","}","~",
							" "
						}
				n = 0
				for i=1, #chars do
					if self.pressedKey~=chars[i] then n = n + 1 end
				end
				if n ~= #chars-1 then self.pressedKey = "" end
			elseif self.mask == "integer" then
				chars =	{
							"-","+",
							"0","1","2","3","4","5","6","7","8","9"
						}
				n = 0
				for i=1, #chars do
					if self.pressedKey==chars[i] then n = 1 end
				end
				if n ~= 1 then self.pressedKey = "" end
				--formats
				if self.pressedKey == "-" or self.pressedKey == "+" then
					if string.sub(self.text,1,1)=="-" and self.pressedKey == "+" then
						self.text = "+"..string.sub(self.text,2,#self.text)
					elseif string.sub(self.text,1,1)=="+" and self.pressedKey == "-" then
						self.text = "-"..string.sub(self.text,2,#self.text)
					end
					self.pressedKey = ""
				else
					if self.pressedKey == "0" then
						if self.vtPosition <= 2 then self.pressedKey = "" end
					end
					if string.sub(self.text,1,1)~="-"and string.sub(self.text,1,1)~="+" then
						if self.text == "0" then
							self.text = "+"
						else
							self.text = "+"..self.text
							self.vtPosition = self.vtPosition + 1
						end
					end
				end
			elseif self.mask == "decimal" then
				chars =	{
							"-","+",".",",",
							"0","1","2","3","4","5","6","7","8","9"
						}
				n = 0
				for i=1, #chars do
					if self.pressedKey==chars[i] then n = 1 end
				end
				if n ~= 1 then self.pressedKey = "" end
				if self.pressedKey == "," then self.pressedKey = "." end
				--formats
				if self.pressedKey == "-" or self.pressedKey == "+" then
					if string.sub(self.text,1,1)=="-" and self.pressedKey == "+" then
						self.text = "+"..string.sub(self.text,2,#self.text)
					elseif string.sub(self.text,1,1)=="+" and self.pressedKey == "-" then
						self.text = "-"..string.sub(self.text,2,#self.text)
					end
					self.pressedKey = ""
				elseif self.pressedKey == "." then
					if findChar(self.text, ".") then
						self.pressedKey=""
					else
						if self.text=="0" then
							self.text="+0"
							self.vtPosition=3
						end
					end
				else
					if self.pressedKey == "0" then
						if self.vtPosition <= 2 then self.pressedKey = "" end
					end
					if string.sub(self.text,1,1)~="-"and string.sub(self.text,1,1)~="+" then
						if self.text == "0" then
							self.text = "+"
						else
							self.text = "+"..self.text
							self.vtPosition = self.vtPosition + 1
						end
					end
				end
				if self.vtPosition == 1 then
					if string.sub(self.text,1,1)=="-" or string.sub(self.text,1,1)=="+" then
						if	self.pressedKey == "1" or self.pressedKey == "2" or
							self.pressedKey == "3" or self.pressedKey == "4" or
							self.pressedKey == "5" or self.pressedKey == "6" or
							self.pressedKey == "7" or self.pressedKey == "8" or
							self.pressedKey == "9" then
								self.vtPosition  = 2
						end
					end
				end
				if self.pressedKey == "-" or  self.pressedKey == "+" then
					if string.sub(self.text,1,1)=="-" or string.sub(self.text,1,1)=="+" then
						self.pressedKey = ""
					elseif string.sub(self.text,1,1)~="-" and string.sub(self.text,1,1)~="+" then
						self.vtPosition = 1
					end
				end
			end
			self.text = tostring(self.text)
			if self.length > #self.text or self.length < 0 then
				self.text = insert(self.text,self.vtPosition,self.pressedKey)
				if self.callbackOnKeyChange ~= nil then self.callbackOnKeyChange(self.pressedKey) end
				self.vtPosition = self.vtPosition + 1
			end
			if jumpLastKey == false then self.lastkey = self.pressedKey end
		else
			if key=="BACKSPACE" and self.shift then self.text = remove(self.text,self.vtPosition,-self.vtPosition)
			elseif key=="BACKSPACE" and not self.shift then self.text = remove(self.text,self.vtPosition)
			end
			if self.callbackOnKeyChange ~= nil then self.callbackOnKeyChange("") end
			self.lastkey = ""
		end
		if self.mask == "integer" or self.mask == "decimal" and self.text == "" or self.text == "+"
			or self.text == "-" or self.text == "." or self.text == "+." or self.text == "-." then
			self.text = "0"
			self.vtPosition = 2
		end
	end
end
function interface_addField:updateKeyReleased(key)
	if self.enabled and self.visible and self.focused then
		if	key == "CAPSLOCK" then self.capslock = not self.capslock
		elseif	key == "LSHIFT" then self.shift = false
		elseif	key == "RSHIFT" then self.shift = false
		end
		if scrupp.keyIsDown("SHIFT") then self.shift = true end
	end
end
function interface_addField:render()
	self.text = tostring(self.text)
	local imageToDraw = 1
	if self.images[1]~=nil then
		if self.focused then imageToDraw = 3
		elseif self.enabled == false then imageToDraw = 2
		end
	end
	if self.visible then
		if self.images[1]==nil then
			local x, y = self.position[1], self.position[2]
			local w, h = self.dimension[1], self.dimension[2]
			--background
			local background_lines = { color = self.bgColor, antialiasing = true, connect = true, fill=true}
			background_lines[#background_lines+1] = x
			background_lines[#background_lines+1] = y
			background_lines[#background_lines+1] = x+w
			background_lines[#background_lines+1] = y
			background_lines[#background_lines+1] = x+w
			background_lines[#background_lines+1] = y+h
			background_lines[#background_lines+1] = x
			background_lines[#background_lines+1] = y+h
			background_lines[#background_lines+1] = x
			background_lines[#background_lines+1] = y
			scrupp.draw(background_lines)
			background_lines = nil
			--border
			local border_lines = { color = self.borderColor, antialiasing = true, connect = true, fill=false}
			border_lines[#border_lines+1] = x
			border_lines[#border_lines+1] = y
			border_lines[#border_lines+1] = x+w
			border_lines[#border_lines+1] = y
			border_lines[#border_lines+1] = x+w
			border_lines[#border_lines+1] = y+h
			border_lines[#border_lines+1] = x
			border_lines[#border_lines+1] = y+h
			border_lines[#border_lines+1] = x
			border_lines[#border_lines+1] = y
			scrupp.draw(border_lines)
			border_lines = nil
		else
			self.images[imageToDraw]:setAlpha(self.alpha)
			self.images[imageToDraw]:render(self.position[1], self.position[2])
		end
		self.text = tostring(self.text)
		self.vtText = self.text
		if self.dimension[1] > 10 then
			--vars
			local textChars = #self.text+1
			local textW, textH = self.font:getTextSize(string.sub(self.text, 1, #self.text))
			local curpos = 1
			local ignoreds = 0
			--
			while curpos + textChars < self.vtPosition - ignoreds do
				ignoreds = ignoreds + 1
			end
			textW, textH = self.font:getTextSize(string.sub(self.text, curpos + ignoreds, curpos + textChars))
			while textW > self.dimension[1] - 6 do
				ignoreds = ignoreds + 1
				textW, textH = self.font:getTextSize(string.sub(self.text, curpos + ignoreds, curpos + textChars))
			end
			if textW>0 then
				self.vtText = string.sub(self.text, curpos + ignoreds, curpos + textChars)
				local textToDraw = self.font:generateImage{self.vtText, color = self.fgColor}
				textToDraw:render(self.position[1]+5, self.position[2]+self.dimension[2]/2-textToDraw:getHeight()/2)
				if self.focused then
					self.tickTime = self.tickTime - 1
					if self.tickTime < 1 then self.tick = not self.tick self.tickTime = 50 end
					if self.tick then
						local pipe_line = { color = self.fgColor, antialiasing = true, connect = true, fill=false}
						local pipeX, pipeY = self.font:getTextSize(string.sub(self.vtText, 1, self.vtPosition - curpos - ignoreds))
						pipe_line[#pipe_line+1] = self.position[1]+5+pipeX
						pipe_line[#pipe_line+1] = self.position[2]+2
						pipe_line[#pipe_line+1] = self.position[1]+5+pipeX
						pipe_line[#pipe_line+1] = self.position[2]-2+self.dimension[2]
						scrupp.draw(pipe_line)
						pipe_line = nil
					end
				end
			else
				if self.focused then
					self.tickTime = self.tickTime - 1
					if self.tickTime < 1 then self.tick = not self.tick self.tickTime = 50 end
					if self.tick then
						local pipe_line = { color = self.fgColor, antialiasing = true, connect = true, fill=false}
						pipe_line[#pipe_line+1] = self.position[1]+5
						pipe_line[#pipe_line+1] = self.position[2]+2
						pipe_line[#pipe_line+1] = self.position[1]+5
						pipe_line[#pipe_line+1] = self.position[2]-2+self.dimension[2]
						scrupp.draw(pipe_line)
						pipe_line = nil
					end
				end
			end
		else
			if self.focused then
				self.tickTime = self.tickTime - 1
				if self.tickTime < 1 then self.tick = not self.tick self.tickTime = 50 end
				if self.tick then
					local pipe_line = { color = self.fgColor, antialiasing = true, connect = true, fill=false}
					pipe_line[#pipe_line+1] = self.position[1]+5
					pipe_line[#pipe_line+1] = self.position[2]+2
					pipe_line[#pipe_line+1] = self.position[1]+5
					pipe_line[#pipe_line+1] = self.position[2]-2+self.dimension[2]
					scrupp.draw(pipe_line)
					pipe_line = nil
				end
			end
		end
		if scrupp.getMouseX()>=self.position[1] and scrupp.getMouseX()<=self.position[1]+self.dimension[1] and scrupp.getMouseY()>=self.position[2] and scrupp.getMouseY()<=self.position[2]+self.dimension[2] then
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
			--Tip
			if self.tip~=nil and self.tipTime>0 then
				self.tip:hide()
				self.tipTime = 0
				if self.connected then self.connector:setTipToRender() end
			end
		end
	end
end
--==================================================================================================================
--End of addField
--==================================================================================================================
--==================================================================================================================
--Begin of addLabel
--==================================================================================================================
function interface_addLabel:remove()
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
function interface_addLabel:setPosition(newPosition)
	--comment to avoid screen limitations
	if newPosition[1]>scrupp.getWindowWidth()-self.dimension[1] then newPosition[1] = scrupp.getWindowWidth()-self.dimension[1] end
	if newPosition[1]<0 then newPosition[1] = 0 end
	if newPosition[2]>scrupp.getWindowHeight()-self.dimension[2] then newPosition[2] = scrupp.getWindowHeight()-self.dimension[2] end
	if newPosition[2]<0 then newPosition[2] = 0 end
	--
	self.position = newPosition
end
function interface_addLabel:getPosition()
	return { x=self.position[1], y=self.position[2] }
end
function interface_addLabel:getDimension()
	return { w=self.dimension[1], h=self.dimension[2] }
end
function interface_addLabel:setDimension(newDimension)
	if type(newDimension[1])~='number' or type(newDimension[2])~='number' then return end
	if newDimension[1]<=0 or newDimension[2]<=0 then return end
	self.dimension = newDimension
end
function interface_addLabel:setAlign(newAlign)
	if newAlign~=nil then
		self.align=string.upper(newAlign)
		if self.align~="L" and  self.align~="C" and  self.align~="R" then
			self.align = "L"
		end
	end
end
function interface_addLabel:getAlign()
	return  self.align
end
function interface_addLabel:setVisible(newState)
	if type(newState)~='boolean' then newState=true end
	self.visible = newState
end
function interface_addLabel:getVisible()
	return self.visible
end
function interface_addLabel:setBoldState(newState)
	if type(newState)~='boolean' then newState=true end
	self.bold = newState
end
function interface_addLabel:getBoldState()
	return self.bold
end
function interface_addLabel:setZ(newZ)
	if newZ == nil then self.z = -1 end
	if type(newZ)~='number' then return end
	if newZ < -1 then self.z = -1 end
	self.z = newZ
end
function interface_addLabel:getZ()
	return self.z
end
function interface_addLabel:setFont(newFontPath, newFontSize)
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
function interface_addLabel:getFont()
	return { name = self.fontName, size = self.fontSize }
end
function interface_addLabel:getFontPath()
	return self.fontPath
end
function interface_addLabel:setText(newText)
	self.text = tostring(newText)
end
function interface_addLabel:getText()
	return self.text
end
function interface_addLabel:Connect(theContainer)
	--theContainer must be a desktop
	if theContainer == nil then return end
	if self.connected == false then theContainer:addControl(self) self.connected=true self.connector=theContainer end
end
function interface_addLabel:Disconnect()
	if self.connected then
		self.connector:removeControl(self)
		self.connected = false
		self.connector = nil
	end
end
function interface_addLabel:setFgColor(newColor)
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
function interface_addLabel:getFgColor()
	return { color = tostring(self.fgColor[1]).." "..tostring(self.fgColor[2]).." "..tostring(self.fgColor[3]), alpha = tostring(self.fgColor[4]) , rgb = {self.fgColor[1], self.fgColor[2], self.fgColor[3]}, a = self.fgColor[4], rgba = {self.fgColor[1], self.fgColor[2], self.fgColor[3], self.fgColor[4]} }
end
function interface_addLabel:updateMousePressed(mouse)
	--pass
end
function interface_addLabel:updateMouseReleased(mouse)
	--pass
end
function interface_addLabel:updateKeyPressed(key)
	--pass
end
function interface_addLabel:updateKeyReleased(key)
	--pass
end
function interface_addLabel:render()
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
--==================================================================================================================
--End of addLabel
--==================================================================================================================
--==================================================================================================================
--Begin of addTip
--==================================================================================================================
function interface_addTip:remove()
	self.image = nil
	self.hasImage = false
	self.alpha = 255
	self.imagePath = ""
	self.fontPath = ""
	self.fontSize = ""
	self.font = nil
	self = nil
end
function interface_addTip:setImage(newImage)
	if newImage == nil then self.hasImage = false self.image = nil self.imagePath = "" return end
	self.image = scrupp.addImage(newImage)
	self.hasImage = true
	self.imagePath = newImage
	self.image:setAlpha(self.alpha)
	self.dimension = { self.image:getWidth(), self.image:getHeight() }
end
function interface_addTip:getImage()
	return self.imagePath
end
function interface_addTip:setAlpha(newAlpha)
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
function interface_addTip:getAlpha()
	return self.alpha
end
function interface_addTip:setFgColor(newColor)
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
function interface_addTip:setBgColor(newColor)
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
function interface_addTip:setBorderColor(newColor)
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
function interface_addTip:setFont(newFontPath, newFontSize)
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
function interface_addTip:getFont()
	return { name = self.fontName, size = self.fontSize }
end
function interface_addTip:getFontPath()
	return self.fontPath
end
function interface_addTip:toggleShowBorder()
	local show = not self.showBorder
	self.showBorder = show
	return show
end
function interface_addTip:getBgColor()
	return { color = tostring(self.bgColor[1]).." "..tostring(self.bgColor[2]).." "..tostring(self.bgColor[3]), alpha = tostring(self.bgColor[4]) , rgb = {self.bgColor[1], self.bgColor[2], self.bgColor[3]}, a = self.bgColor[4], rgba = {self.bgColor[1], self.bgColor[2], self.bgColor[3], self.bgColor[4]} }
end
function interface_addTip:getFgColor()
	return { color = tostring(self.fgColor[1]).." "..tostring(self.fgColor[2]).." "..tostring(self.fgColor[3]), alpha = tostring(self.fgColor[4]) , rgb = {self.fgColor[1], self.fgColor[2], self.fgColor[3]}, a = self.fgColor[4], rgba = {self.fgColor[1], self.fgColor[2], self.fgColor[3], self.fgColor[4]} }
end
function interface_addTip:getBorderColor()
	return { color = tostring(self.borderColor[1]).." "..tostring(self.borderColor[2]).." "..tostring(self.borderColor[3]), alpha = tostring(self.borderColor[4]) , rgb = {self.borderColor[1], self.borderColor[2], self.fgColor[3]}, a = self.borderColor[4], rgba = {self.borderColor[1], self.borderColor[2], self.borderColor[3], self.borderColor[4]} }
end
function interface_addTip:hide()
	self.isShowing = false
end
function interface_addTip:show()
	self.isShowing = true
end
function interface_addTip:render(x, y, text)
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
--==================================================================================================================
--Begin of addTip
--==================================================================================================================
--==================================================================================================================
--Begin of addDesktop
--==================================================================================================================
function interface_addDesktop:setTipToRender(theTip, x, y, text)
	if theTip==nil then self.tipToRender = {nil, -1, -1, "" } return end
	self.tipToRender = { theTip, x, y, text }
end
function interface_addDesktop:setBgColor(newColor)
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
function interface_addDesktop:remove()
	self.controls = nil
	if self.hasImage then
		self.imagePath = ""
		self.image = nil
	end
	self = nil
end
function interface_addDesktop:toggleShowBgColor()
	local show = not self.showBack
	self.showBack = show
	return show
end
function interface_addDesktop:getBgColor()
	return { color = tostring(self.bgColor[1]).." "..tostring(self.bgColor[2]).." "..tostring(self.bgColor[3]), alpha = tostring(self.bgColor[4]) , rgb = {self.bgColor[1], self.bgColor[2], self.bgColor[3]}, a = self.bgColor[4], rgba = {self.bgColor[1], self.bgColor[2], self.bgColor[3], self.bgColor[4]} }
end
function interface_addDesktop:hasControl(theControl)
	for i=1,#self.controls do
		if self.controls[i]==theControl then return true end
	end
	return false
end
function interface_addDesktop:addControl(theControl)
	if theControl==nil then return end
	for i=1,#self.controls do
		if self.controls[i]==theControl then return end
	end
	self.controls[#self.controls+1] = theControl
end
function interface_addDesktop:removeControl(theControl)
	if theControl==nil then return end
	for i=1,#self.controls do
		if self.controls[i]==theControl then table.remove(self.controls, i) return end
	end
end
function interface_addDesktop:setImage(newImage)
	if newImage == nil then self.hasImage = false self.image = nil self.imagePath = "" return end
	self.image = scrupp.addImage(newImage)
	dimension = { self.image:getWidth(), self.image:getHeight() }
	self.hasImage = true
	self.imagePath = newImage
	newDimension = { scrupp.getWindowWidth(), scrupp.getWindowHeight() }
	self.image:setScale((newDimension[1]*self.image:getScaleX())/dimension[1], (newDimension[2]*self.image:getScaleY())/dimension[2])
end
function interface_addDesktop:getImage()
	return self.imagePath
end
function interface_addDesktop:setAlpha(newAlpha)
	if type(newAlpha)~='number' then return end
	if newAlpha >= 0 and newAlpha <= 255 then
		self.alpha = newAlpha
	else
		self.alpha = 255 --default
	end
end
function interface_addDesktop:getAlpha()
	return self.alpha
end
function interface_addDesktop:setVisible(newState)
	if type(newState)~='boolean' then newState=true end
	self.visible = newState
end
function interface_addDesktop:getVisible()
	return self.visible
end
function interface_addDesktop:update()
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
function interface_addDesktop:updateMousePressed(mouse)
	if self.visible then
		for i=1,#self.controls do
			if self.controls[i]~=nil then
				self.controls[i]:updateMousePressed(mouse)
			end
		end
	end
end
function interface_addDesktop:updateMouseReleased(mouse)
	if self.visible then
		for i=1,#self.controls do
			if self.controls[i]~=nil then
				self.controls[i]:updateMouseReleased(mouse)
			end
		end
	end
end
function interface_addDesktop:updateKeyPressed(key)
	if self.visible then
		for i=1,#self.controls do
			if self.controls[i]~=nil then
				self.controls[i]:updateKeyPressed(key)
			end
		end
	end
end
function interface_addDesktop:updateKeyReleased(key)
	if self.visible then
		for i=1,#self.controls do
			if self.controls[i]~=nil then
				self.controls[i]:updateKeyReleased(key)
			end
		end
	end
end
--==================================================================================================================
--End of addDesktop
--==================================================================================================================
--==================================================================================================================
--Begin of addButton
--==================================================================================================================
function interface_addButton:removeTip()
	self.tip = nil
	self.tipText = ""
	self.tipDelay = 250
	self.tipTime = 0
end
function interface_addButton:getTip()
	if self.tip==nil then return end
	return { tip = self.tip, text = self.tipText, delay = self.tipDelay }
end
function interface_addButton:Tip(theTip, text, delay)
	if self.tip~=nil then
		if theTip==self.tip then return end
	end
	if theTip~=nil then self.tip = theTip end
	self.tipText = tostring(text)
	if type(delay)=="number" then self.tipDelay = delay end
	self.tipTime = 0
end
function interface_addButton:setDimension(newDimension)
	if type(newDimension[1])~='number' or type(newDimension[2])~='number' then return end
	if newDimension[1]<=0 or newDimension[2]<=0 then return end
	for i=1,4 do
		self.images[i] = theme.createButtonImage(newDimension[1], newDimension[2], styles[i])
		--self.images[i]:setScale((newDimension[1]*self.images[i]:getScaleX())/self.dimension[1], (newDimension[2]*self.images[i]:getScaleY())/self.dimension[2])
	end
	self.dimension = newDimension
end
function interface_addButton:setZ(newZ)
	if newZ == nil then self.z = -1 end
	if type(newZ)~='number' then return end
	if newZ < -1 then self.z = -1 end
	self.z = newZ
end
function interface_addButton:getZ()
	return self.z
end
function interface_addButton:Connect(theContainer)
	--theContainer must be a desktop
	if theContainer == nil then return end
	if self.connected == false then theContainer:addControl(self) self.connected=true self.connector=theContainer end
end
function interface_addButton:Disconnect()
	if self.connected then
		self.connector:removeControl(self)
		self.connected = false
		self.connector = nil
	end
end
function interface_addButton:getAlpha()
	return self.alpha
end
function interface_addButton:remove()
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
function interface_addButton:setAlpha(newAlpha)
	if type(newAlpha)~='number' then return end
	if newAlpha >= 0 and newAlpha <= 255 then
		self.alpha = newAlpha
	else
		self.alpha = 255 --default
	end
end
function interface_addButton:setImages(newImages)
	if newImages[1] == nil then return end
	if newImages[2]==nil then newImages[2] = newImages[1] end
	if newImages[3]==nil then newImages[3] = newImages[1] end
	if newImages[4]==nil then newImages[4] = newImages[1] end
	self.images = {newImages[1],newImages[2],newImages[3],newImages[4]}
	self.dimension = { self.images[1]:getWidth(), self.images[1]:getHeight() }
end
function interface_addButton:getImages()
	return {normal = self.images[1], disabled = self.images[2], pressed = self.images[3], focused = self.images[4]}
end
function interface_addButton:setKeys(newKeys)
	if newKeys[1] == nil then return end
	self.keys = newKeys
end
function interface_addButton:setVisible(newState)
	if type(newState)~='boolean' then newState=true end
	self.visible = newState
end
function interface_addButton:getVisible()
	return self.visible
end
function interface_addButton:getKeys()
	return self.keys
end
function interface_addButton:setPosition(newPosition)
	--comment to avoid screen limitations
	if newPosition[1]>scrupp.getWindowWidth()-self.dimension[1] then newPosition[1] = scrupp.getWindowWidth()-self.dimension[1] end
	if newPosition[1]<0 then newPosition[1] = 0 end
	if newPosition[2]>scrupp.getWindowHeight()-self.dimension[2] then newPosition[2] = scrupp.getWindowHeight()-self.dimension[2] end
	if newPosition[2]<0 then newPosition[2] = 0 end
	--
	self.position = newPosition
end
function interface_addButton:getPosition()
	return { x=self.position[1], y=self.position[2] }
end
function interface_addButton:getDimension()
	return { w=self.dimension[1], h=self.dimension[2] }
end
function interface_addButton:setEnabled(newState)
	if type(newState)~='boolean' then newState=true end
	self.enabled = newState
end
function interface_addButton:getEnabled()
	return self.enabled
end
function interface_addButton:setText(newText)
	self.text = tostring(newText)
end
function interface_addButton:getText()
	return self.text
end
function interface_addButton:getFont()
	return { name = self.fontName, size = self.fontSize }
end
function interface_addButton:getFontPath()
	return self.fontPath
end
function interface_addButton:setFont(newFontPath, newFontSize)
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
function interface_addButton:setFgColor(newColor)
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
function interface_addButton:getFgColor()
	return { color = tostring(self.fgColor[1]).." "..tostring(self.fgColor[2]).." "..tostring(self.fgColor[3]), alpha = tostring(self.fgColor[4]) , rgb = {self.fgColor[1], self.fgColor[2], self.fgColor[3]}, a = self.fgColor[4], rgba = {self.fgColor[1], self.fgColor[2], self.fgColor[3], self.fgColor[4]} }
end
function interface_addButton:updateMousePressed(mouse)
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
function interface_addButton:updateMouseReleased(mouse)
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
function interface_addButton:updateKeyPressed(key)
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
function interface_addButton:updateKeyReleased(key)
	if #self.keys == 0 then return end
	key = string.lower(key)
	for i=1, #self.keys do
		if key == string.lower(self.keys[i]) then
			self.numberpressed = self.numberpressed - 1
		end
	end
end
function interface_addButton:render()
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
--==================================================================================================================
--End of addButton
--==================================================================================================================
