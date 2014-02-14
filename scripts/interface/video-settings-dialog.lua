--[[
    $Id$
    Video Settings Dialog for setting resolution and fullscreen/windows mode.
    See examples/videoSettingsDialog.slua for a demo.
]]--

require "interface"

local videoSettingsDialog = {}

local btn1, btn2, btn3

local sformat = string.format

local function mode2string(mode)
    return sformat("%d x %d", mode[1], mode[2])
end

function videoSettingsDialog.show(modes, callback)
    if type(modes) ~= "table" then
        error("table expected")
    end
    local count = #modes
    if count == 0 then
        error("no video modes available")
    end
    if count == 1 then return modes[1] end

    scrupp.init("Video Settings", 110, 190, 32, false)

    background = interface.addDesktop()
    background:setBgColor{255, 255, 255}

    btn1 = interface.addButton(function()
        if btn1:getText() == "Fullscreen Mode" then
            btn1:setText("Windowed Mode")
        else
            btn1:setText("Fullscreen Mode")
        end
    end)
    btn1:setText("Windowed Mode")
    btn1:setPosition{5, 10}
    btn1:Connect(background)

    local index = 1
    btn2 = interface.addButton(function()
        index = index + 1
        if index > count then
            index = 1
        end
        btn2:setText(mode2string(modes[index]))
    end)
    btn2:setText(mode2string(modes[1]))
    btn2:setPosition{ btn1:getPosition().x, btn1:getPosition().y+50 }
    btn2:Connect(background)

    btn3 = interface.addButton(function()
        callback(modes[index][1], modes[index][2], btn1:getText() == "Fullscreen Mode")
    end)
    btn3:setText("OK")
    btn3:setPosition{ btn2:getPosition().x, btn2:getPosition().y+80 }
    btn3:setKeys{"RETURN"}
    btn3:Connect(background)


    main = {
        render = function(dt)
            background:update()
        end,
        keypressed = function(key)
            if key == "ESCAPE" then
                scrupp.exit()
            end
            background:updateKeyPressed(key)
        end,
        keyreleased = function(key)
            background:updateKeyReleased(key)
        end,
        mousereleased = function(x, y, button)
            background:updateMouseReleased(button)
        end,
        mousepressed = function(x, y, button)
            background:updateMousePressed(button)
        end
    }
end


return videoSettingsDialog
