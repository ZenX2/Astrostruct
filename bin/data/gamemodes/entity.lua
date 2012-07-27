-- don't edit this please, it's used by the engine to register things.

module("entity")

local Entities = {}

function get(ClassName)
	local Entity = Entities[ClassName]
	if (not Entity) then
		return nil
	end
	return Entity
end

function register(Entity, ClassName)
	if (get(ClassName) ~= nil) then
		return
	end
	Entities[ClassName] = Entity
end
