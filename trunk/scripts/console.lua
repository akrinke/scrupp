--[[
-- $Id$
-- Console class
-- See Copyright Notice in COPYRIGHT
--]]

require "class"
require "font"

local prompt = "> "
local font = "fonts/VeraMono.ttf"
local font_size = 14

local left_margin = 10
local right_margin = 10
local bg_color = {64, 64, 64, 160}
local text_color = {255, 255, 255}
local cursor_color = {255, 255, 255, 64}

local mfloor = math.floor
local mceil = math.ceil
local mmax = math.max
local mmin = math.min
local sformat = string.format
local ssub = string.sub
local tconcat = table.concat
local tinsert = table.insert
local tremove = table.remove

-- returns the result and the concatenation of all additional arguments
local function collect(result, ...)
	local msg = {}
	for i=1, select("#", ...) do
		msg[#msg+1] = tostring(select(i, ...))
	end
	msg = tconcat(msg, ", ")
	return result, msg
end

-- splits string s in multiple lines and adds it to table t
local function split_in_lines(s, t)
	for l in string.gmatch(tostring(s), "[^\n]+") do
		t[#t+1] = l
	end
end

-- splits string s in multiple lines with less then c characters
local function split_in_fitting_lines(s, c)
	-- how many lines are nescessary?
	local n = mceil(#s / c)
	local lines = {}
	for i=1,n do
		lines[#lines+1] = ssub(s, (i-1)*c+1, i*c)
	end
	return lines
end

-- new print function that outputs to a global variable
-- instead of stdout
local function new_print(...)
	for i=1, select("#", ...) do
		scrupp._tmp = scrupp._tmp .. tostring(select(i, ...)) .. "\t"
	end
	scrupp._tmp = scrupp._tmp .. "\n"
end

-- new io.write function that outputs to a global variable
-- instead of stdout
local function new_io_write(...)
	for i=1, select("#", ...) do
		scrupp._tmp = scrupp._tmp .. tostring(select(i, ...))
	end
end

Console = class(function(self)
	self.active = false
	-- command history
	self.history = {}
	self.history_idx = 0
	-- lines
	self.lines = {}
	-- can the user scroll up via CTRL+UP?
	self.scrollable = false
	-- how many lines scrolled up?
	self.scrolled = 0
	-- position of the cursor
	self.cursor_pos = 1
	-- load font
	self.font = Font(font, font_size)
	-- define the cursor
	local cursor_width = self.font:getTextSize("_")
	local cursor_height = self.font:getHeight()
	self.cursor_width = cursor_width
	self.cursor = {
		0, 0,
		0, 0,
		cursor_width,0,
		cursor_width, cursor_height,
		0,cursor_height,
		0, 0,
		relative = true,
		connect = true,
		fill = true,
		color = cursor_color
	}
	-- table with all characters of the command
	self.tcommand = {}
	-- the command (concatenation of tcommand)
	self.command = ""
	-- holds the current tcommand, if the history is used
	self.tcurrent_command = nil
end)

function Console:isActive()
	return self.active
end

function Console:print(...)
	local s = ""
	local n = select("#", ...)
	for i=1,n do
		s = s .. tostring(select(i, ...))
		if i~=n then
			s = s .. "\t"
		end
	end
	self.lines[#self.lines+1] = s
end

function Console:printf(...)
	self.lines[#self.lines+1] = sformat(...)
end

function Console:keypressed(key, wchar)
	if key == "^" or key == "`" then
		self.active = not self.active
		if self.active then
			self.unicodeWasEnabled = scrupp.unicodeIsEnabled()
			scrupp.enableUnicode()
		elseif not self.unicodeWasEnabled then
			scrupp.disableUnicode()
		end
		return "", ""
	elseif self.active then
		-- handle input
		if	   key == "BACKSPACE" then
			-- remove character left of cursor
			if self.cursor_pos > 1 then
				self.cursor_pos = self.cursor_pos - 1
				tremove(self.tcommand, self.cursor_pos)
				self.command = tconcat(self.tcommand)
			end
		elseif key == "DELETE" then
			-- remove character under cursor
			tremove(self.tcommand, self.cursor_pos)
			self.command = tconcat(self.tcommand)
		elseif key == "RETURN" or key == "KP_ENTER" then
			-- execute command

			-- put the command at the front of the history,
			-- if it is different from the last one
			if not self.history[1] or tconcat(self.tcommand) ~= tconcat(self.history[1]) then
				tinsert(self.history, 1, self.tcommand)
			end
			self.history_idx = 0
			-- put the whole line in the list of old lines
			self.lines[#self.lines+1] = prompt .. self.command
			if self.command == "clear" then
				self.lines = {}
			else
				scrupp._tmp = ""
				local old_print = print
				local old_io_write = io.write
				print = new_print
				io.write = new_io_write
				local result, msg = loadstring(self.command:gsub("^=", "return "), "command")
				if result then
					-- collect all return values of pcall
					result, msg = collect(pcall(result))
				end
				print = old_print
				io.write = old_io_write
				
				if not result then
					-- make the error message a little bit shorter
					local idx = msg:find(": ")
					if idx then
						msg = msg:sub(idx+2)
					end
					if msg == "scrupp.exit" then
						-- scrupp.exit() was called
						scrupp.exit()
					end
				end
				-- 1. the output
				if scrupp._tmp ~= "" then
					print(scrupp._tmp)
					split_in_lines(scrupp._tmp, self.lines)
				end
				-- 2. the return values
				split_in_lines(msg, self.lines)
			end
			self.tcommand = {}
			self.command = ""
			self.cursor_pos = 1
		elseif key == "ESCAPE" then
			self.active = false
		elseif key == "TAB" then
			-- tab completion
		elseif key == "LEFT" then
			-- move cursor to the left
			self.cursor_pos = mmax(self.cursor_pos - 1, 1)
		elseif key == "RIGHT" then
			-- move cursor to the right
			self.cursor_pos = mmin(self.cursor_pos + 1, #self.tcommand + 1)
		elseif key == "HOME" then
			self.cursor_pos = 1
		elseif key == "END" then
			self.cursor_pos = #self.tcommand + 1
		elseif key == "UP" then
			if scrupp.keyIsDown("CTRL") then
				-- go up one line
				if self.scrollable then
					self.scrolled = self.scrolled + 1
				end
			elseif self.history_idx < #self.history then
				-- go back in the history
				if self.history_idx == 0 then
					-- save the current tcommand
					self.tcurrent_command = self.tcommand
				end
				self.history_idx = self.history_idx + 1
				local tcommand = {}
				local history_tcommand = self.history[self.history_idx]
				for i=1,#history_tcommand do
					tcommand[i] = history_tcommand[i]
				end
				self.tcommand = tcommand
				self.command = tconcat(tcommand)
				self.cursor_pos = #tcommand + 1
			end
		elseif key == "DOWN" then
			if scrupp.keyIsDown("CTRL") then
				-- go down one line
				self.scrolled = mmax(self.scrolled - 1, 0)
			elseif self.history_idx > 0 then
				-- go forward in the history
				self.history_idx = self.history_idx - 1
				if self.history_idx == 0 then
					self.tcommand = self.tcurrent_command
				else
					self.tcommand = self.history[self.history_idx]
				end
				self.command = tconcat(self.tcommand)
				self.cursor_pos = #self.tcommand + 1
			end
		-- CTRL+Key does not generate input
		-- ALT GR+Key (which equals CTRL+ALT+Key) does generate input
		elseif wchar ~= "" and (not scrupp.keyIsDown("CTRL") or scrupp.keyIsDown("RALT"))  then
			tinsert(self.tcommand, self.cursor_pos, wchar)
			self.command = tconcat(self.tcommand)
			self.cursor_pos = self.cursor_pos + 1
		end
		
		if not scrupp.keyIsDown("CTRL") then
			-- scroll to prompt
			self.scrolled = 0
		end
		
		return "", ""
	end
	return key, wchar
end

function Console:render()
	if not self.active then return end

	local font = self.font
	local line_skip = font:getLineSkip()
	local cursor = self.cursor
	local char_width = self.cursor_width
	-- size of the window
	local window_width = scrupp.getWindowWidth()
	local window_height = scrupp.getWindowHeight()
	-- how many characters fit in one line?
	local width = mfloor((window_width- left_margin - right_margin) / char_width)
	-- y-coordinate of the lowest line
	local lowest_line_y = window_height - 2 * line_skip
	-- current y-coordinate
	local y = lowest_line_y
	
	self.scrollable = false
	-- how many lines scrolled up?
	local scrolled = self.scrolled
	-- current line number
	local line = 1

	-- render background
	scrupp.draw{
		0,            0,
		window_width, 0,
		window_width, window_height,
		0,            window_height,
		fill = true,
		color = bg_color
	}

	-- print current line
	local command = split_in_fitting_lines(prompt .. self.command, width)
	for i=#command, 1, -1 do
		if line > scrolled then
			if y < 0 then
				print("scrollable")
				self.scrollable = true
				break
			end
			font:print(left_margin, y, command[i])
			y = y - line_skip
		end
		line = line + 1
	end

	-- rendering the cursor
	if scrolled == 0 then
		-- x-coordinate
		cursor[1] = left_margin + ((self.cursor_pos % width) - 1 + #prompt) * char_width
		-- y-coordinate
		cursor[2] = lowest_line_y
		scrupp.draw(self.cursor)
	end

	-- print all old lines
	if #self.lines > 0 then
		for i=#self.lines, 1, -1 do
			if y < 0 then
				self.scrollable = true
				break
			end
			command = split_in_fitting_lines(self.lines[i], width)
			for j=#command, 1, -1 do
				if line > scrolled then
					if y < 0 then
						self.scrollable = true
						break
					end
					font:print(left_margin, y, command[j])
					y = y - line_skip
				end
				line = line + 1
			end
		end
	end
	
	if y > 0 then
		self.scrolled = mmax(self.scrolled - 1, 0)
	end
end
