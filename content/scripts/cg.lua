package.path = package.path .. ";C:/lua/lua/?.lua"
package.cpath = package.cpath .. ";C:/lua/odbc/core.dll"

--https://github.com/moteus/lua-odbc
odbc = require "odbc"
--json = require ("dkjson")

store_procedure_name = ""
SPLIT = "%*#?~!#*%"
error  = "true"
result = ""

--function test()
	 -- file = io.open("c:/test/out.txt", "w")
	  --io.output(file)
	  --io.write(data)
	  --io.close(file)
--end

function connect()
	  error = "true"
	  result = ""

	  if store_procedure_name == "" then
			return
	  end

      local _connection_str = {
		   {Driver = '{SQL Server}'};
		   {Server='192.168.0.130'};
		   {Port='1433'};
		   {Database='NRCSDB'};
		   {Uid='Newsroom'};
		   {Pwd='1qaz!QAZ'};
	  };
	   _con = odbc.connect(_connection_str)
	   _collection = assert(_con:prepare(store_procedure_name))
	   assert(_collection:execute())

	   _collection:foreach(
		function(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)
			result = result .. tostring(d)
			result = result .. SPLIT
		end)

	  -- _c = assert(_con:prepare("{call Cg.AddObject(?,?,?)}"))
	  -- _c:vbind_param_ulong(1, 2)
	  -- _c:vbind_param_char(2, "test")
	  -- _c:vbind_param_wchar(3, "سلام") 
	  -- _c:execute()

	   error = "false"
end



--local tbl = {
  --animals = { "dog", "cat", "aardvark" },
  --instruments = { "سلام خویسسییس", "tromasdadsdasdbone", "th   asdda    asdasd eremin" },
  --bugs = json.null,
  --trees = nil
--}
--str = json.encode (tbl, { indent = true })
--print (str)
--print(string.len(str))
 
-- Opens a file in append mode
--file = io.open("out.txt", "w")

-- sets the default output file as test.lua
--io.output(file)

-- appends a word test to the last line of the file
--io.write(str)
--stmt:foreach(function(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)
   --io.write(tostring(a))
   --io.write("\r\n")
   --io.write(tostring(b))
   --io.write("\r\n")
   --io.write(tostring(c))
   --io.write("\r\n")
   --io.write(tostring(d))
   --io.write("\r\n")
   --io.write(tostring(e))
   --io.write("\r\n")
   --io.write(tostring(f))
   --io.write("\r\n")
   --io.write("\r\n")
   --str = b
   --io.write(c)
   --io.write(d)
   --io.write(o)
   --print(f)
--end)
 
-- closes the open file
--io.close(file)

