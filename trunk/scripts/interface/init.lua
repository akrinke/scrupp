--[[
-- $Id$
-- Interface for Scrupp; by: Everton Costa
-- Main Lua file of the module
-- See Copyright Notice in COPYRIGHT
--]]

print("Uses Interface for Scrupp; by: Everton Costa")

local require = require

module(...)

addField   = require "interface.field"
addLabel   = require "interface.label"
addTip     = require "interface.tip"
addDesktop = require "interface.desktop"
addButton  = require "interface.button"