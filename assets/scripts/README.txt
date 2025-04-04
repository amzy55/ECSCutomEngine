All scripts (that operate on entities) must have the same structure:

Entity = {}
Entity.__index = Entity

function Entity.new(entity)
	local o = {}
	setmetatable(o, Entity)
	-- optional initialization of variables
end

function Entity:update(entity, dt)
	-- behaviour
end

return Entity

The "new" function - this acts as the constructor and can be left as it is in the example, or used to initialize varaibles. 

The "update" function - this is to give the behavior of the entity - it will be called every frame.

The base structure of a script can be seen in "templa_script.lua", or in a practical example in "camera_script.lua".

If you want to see what functions/functionalities from the engine can be used in lua, you can look in the include/scripting folder.

Finally, in oder to attach a script to an enitity, the entity must have a ScriptComponent containing the file path of the script.