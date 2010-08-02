--[[
-- $Id$
-- Interface for Scrupp; by: Everton Costa
-- Implements some utility functions
-- See Copyright Notice in COPYRIGHT
--]]

local string = string
local table = table
local tostring = tostring

module(...)

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
