--[[
-- $Id$
-- Interface for Scrupp; by: Everton Costa
-- Field class
-- See Copyright Notice in COPYRIGHT
--]]

require "class"
require "interface.utils"

local split = interface.utils.split
local findDigitAndReplace = interface.utils.findDigitAndReplace
local findChar = interface.utils.findChar
local insert = interface.utils.insert
local remove = interface.utils.remove

local Field = class(function(self, text, font, images, onKeyEnter, onChange)
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
end)

function Field:setLength(newLength)
	if tostring(newLength)=="" or tostring(newLength)=="*" then newLength  = -1 end
	if type(newLength) ~= "number" then return end
	newLength = math.abs(newLength)
	if newLength < 0 then
		self.length = -1
	else
		self.length = newLength
	end
end

function Field:getLength()
	return self.length
end

function Field:getValue()
	if self.mask == "ASCII" or self.mask == "UTF-8" then
		return tostring(self.text)
	else
		return tonumber(self.text)
	end
end

function Field:setMask(newMask)
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

function Field:getMask()
	return self.mask
end

function Field:setKeyboard(newKeyboardType)
	if tostring(newKeyboardType)=="1" then newKeyboardType = "PS/2"
	elseif tostring(newKeyboardType)=="2" then newKeyboardType = "scrupp" end
	if string.upper(newKeyboardType)=="PS/2" or string.lower(newKeyboardType)=="scrupp" then
		self.keyboard = newKeyboardType
	end
end

function Field:getKeyboard()
	return self.keyboard
end

function Field:removeTip()
	self.tip = nil
	self.tipText = ""
	self.tipDelay = 250
	self.tipTime = 0
end

function Field:getTip()
	if self.tip==nil then return end
	return { tip = self.tip, text = self.tipText, delay = self.tipDelay }
end

function Field:Tip(theTip, text, delay)
	if self.tip~=nil then
		if theTip==self.tip then return end
	end
	if theTip~=nil then self.tip = theTip end
	self.tipText = tostring(text)
	if type(delay)=="number" then self.tipDelay = delay end
	self.tipTime = 0
end

function Field:setDimension(newDimension)
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

function Field:setZ(newZ)
	if newZ == nil then self.z = -1 end
	if type(newZ)~='number' then return end
	if newZ < -1 then self.z = -1 end
	self.z = newZ
end

function Field:getZ()
	return self.z
end

function Field:Connect(theContainer)
	--theContainer must be a desktop
	if theContainer == nil then return end
	if self.connected == false then theContainer:addControl(self) self.connected=true self.connector=theContainer end
end

function Field:Disconnect()
	if self.connected then
		self.connector:removeControl(self)
		self.connected = false
		self.connector = nil
	end
end

function Field:getAlpha()
	return self.alpha
end

function Field:remove()
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

function Field:setAlpha(newAlpha)
	if type(newAlpha)~='number' then return end
	if newAlpha >= 0 and newAlpha <= 255 then
		self.alpha = newAlpha
	else
		self.alpha = 255 --default
	end
end

function Field:setImages(newImages)
	if newImages[1] == nil then return end
	if newImages[2]==nil then newImages[2] = newImages[1] end
	if newImages[3]==nil then newImages[3] = newImages[1] end
	self.images = {newImages[1],newImages[2],newImages[3]}
	self.dimension = { self.images[1]:getWidth(), self.images[1]:getHeight() }
end

function Field:getImages()
	return {normal = self.images[1], disabled = self.images[2], focused = self.images[3]}
end

function Field:setVisible(newState)
	if type(newState)~='boolean' then newState=true end
	self.visible = newState
end

function Field:getVisible()
	return self.visible
end

function Field:setPosition(newPosition)
	--comment to avoid screen limitations
	if newPosition[1]>scrupp.getWindowWidth()-self.dimension[1] then newPosition[1] = scrupp.getWindowWidth()-self.dimension[1] end
	if newPosition[1]<0 then newPosition[1] = 0 end
	if newPosition[2]>scrupp.getWindowHeight()-self.dimension[2] then newPosition[2] = scrupp.getWindowHeight()-self.dimension[2] end
	if newPosition[2]<0 then newPosition[2] = 0 end
	--
	self.position = newPosition
end

function Field:getPosition()
	return { x=self.position[1], y=self.position[2] }
end

function Field:getDimension()
	return { w=self.dimension[1], h=self.dimension[2] }
end

function Field:setEnabled(newState)
	if type(newState)~='boolean' then newState=true end
	self.enabled = newState
end

function Field:getEnabled()
	return self.enabled
end

function Field:setText(newText)
	self.text = tostring(newText)
end

function Field:getText()
	return self.text
end

function Field:getFont()
	return { name = self.fontName, size = self.fontSize }
end

function Field:getFontPath()
	return self.fontPath
end

function Field:setFont(newFontPath, newFontSize)
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

function Field:setFgColor(newColor)
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

function Field:getFgColor()
	return { color = tostring(self.fgColor[1]).." "..tostring(self.fgColor[2]).." "..tostring(self.fgColor[3]), alpha = tostring(self.fgColor[4]) , rgb = {self.fgColor[1], self.fgColor[2], self.fgColor[3]}, a = self.fgColor[4], rgba = {self.fgColor[1], self.fgColor[2], self.fgColor[3], self.fgColor[4]} }
end

function Field:setBgColor(newColor)
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

function Field:getBgColor()
	return { color = tostring(self.bgColor[1]).." "..tostring(self.bgColor[2]).." "..tostring(self.bgColor[3]), alpha = tostring(self.bgColor[4]) , rgb = {self.bgColor[1], self.bgColor[2], self.bgColor[3]}, a = self.bgColor[4], rgba = {self.bgColor[1], self.bgColor[2], self.bgColor[3], self.bgColor[4]} }
end

function Field:setBorderColor(newColor)
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

function Field:getBorderColor()
	return { color = tostring(self.borderColor[1]).." "..tostring(self.borderColor[2]).." "..tostring(self.borderColor[3]), alpha = tostring(self.borderColor[4]) , rgb = {self.borderColor[1], self.borderColor[2], self.borderColor[3]}, a = self.borderColor[4], rgba = {self.borderColor[1], self.borderColor[2], self.borderColor[3], self.borderColor[4]} }
end

function Field:updateMousePressed(mouse)
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

function Field:updateMouseReleased(mouse)
	--pass
end

function Field:updateKeyPressed(key)
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

function Field:updateKeyReleased(key)
	if self.enabled and self.visible and self.focused then
		if	key == "CAPSLOCK" then self.capslock = not self.capslock
		elseif	key == "LSHIFT" then self.shift = false
		elseif	key == "RSHIFT" then self.shift = false
		end
		if scrupp.keyIsDown("SHIFT") then self.shift = true end
	end
end

function Field:render()
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

return Field
