-- https://web.archive.org/web/20041211184550/http://alx2002.free.fr/esoterism/brainfuck/brainfuck_en.html#exp1
-- https://es.wikipedia.org/wiki/Brainfuck
local filePath = ({...})[1]
local code = ""
local file = io.open(filePath,"r")
if file then
    code = file:read("*all")
else
    print("Unable to open "..filePath)
end

local luacode = "local i, c = 0, {}\n"
for char in code:gmatch(".") do
    if char == ">" then luacode = luacode .. "i = i + 1\n"
    elseif char == "<" then luacode = luacode .. "i = i - 1\n"
    elseif char == "+" then luacode = luacode .. "c[i] = (c[i] or 0) + 1\n"
    elseif char == "-" then luacode = luacode .. "c[i] = (c[i] or 0) - 1\n"
    elseif char == "[" then luacode = luacode .. "while (c[i] or 0) ~= 0 do\n"
    elseif char == "]" then luacode = luacode .. "end\n"
    elseif char == "." then luacode = luacode .. "io.write(string.char(c[i] or 0))\n"
    elseif char == "," then luacode = luacode .. "c[i] = io.read():byte()\n"
    end
end

f = io.open("out.lua", "w")
f:write(luacode)
f:close()

