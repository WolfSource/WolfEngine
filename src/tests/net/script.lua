function tellme()
io.write("This is coming from lua.tellme.\n")
end

function square(n)
io.write("Within callfuncscript.lua fcn square, arg=")
io.write(tostring(n))
n = n * n
m=n+n
o=n-n
io.write(", square=")
io.write(tostring(n))
io.write(tostring(m))
io.write(tostring(o))

print(".")
return n,m,o
end
-- call the registered C-function
io.write('[Lua] Calling the C functionn')
a,b = displayLuaFunction(12, 3.141592, 'hola')

-- print the return values
io.write('[Lua] The C function returned <' .. a .. '> and <' .. b .. '>n')