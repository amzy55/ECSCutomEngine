Entity = {}
Entity.__index = Entity

function Entity.new(entity)
	local o = {}
	setmetatable(o, Entity)
end

function Entity:update(entity, dt)
	
end

return Entity