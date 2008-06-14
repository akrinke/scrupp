--[[
-- $Id$
-- Very simple greeting program
-- Started if Scrupp is run without arguments
-- See Copyright Notice in COPYRIGHT
--]]

local width, height = 640, 480

game.init("Scrupp", width, height, 32, false)

-- loading fonts
Fonts = {
	game.addFont("fonts/Vera.ttf", 28),
	game.addFont("fonts/Winks.ttf", 150),
}

-- generating images
Images = {
	Fonts[1]:generateImage{"Welcome to"},
	Fonts[2]:generateImage{"Scrupp", color={190,0,0}},
	Fonts[1]:generateImage{"Just change 'main.lua' to suit your needs!"}
}

main = {
	render = function(dt)
		Images[1]:render((width-Images[1]:getWidth())/2,80)
		Images[2]:render((width-Images[2]:getWidth())/2,150)
		Images[3]:render((width-Images[3]:getWidth())/2,350)
	end,

	keypressed = function(k)
		if k == key.ESCAPE then
			game.exit()
		end
	end
}
