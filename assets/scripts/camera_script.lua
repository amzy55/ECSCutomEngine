Camera = {}
Camera.__index = Camera

function Camera.new()
	local o = {}
	setmetatable(o, Camera)

	return o
end

function Camera:update(entity, dt)
	if GetMouseButton(MouseButton.Right) then
		local translateCam = vec3.new(0, 0, 0)
		local speed = entity:GetCamera().speed
		if GetKeyboardKey(KeyboardKey.W) then
			translateCam.z = translateCam.z + speed * dt
		end
		if GetKeyboardKey(KeyboardKey.S) then
			translateCam.z = translateCam.z - speed * dt
		end
		if GetKeyboardKey(KeyboardKey.A) then
			translateCam.x = translateCam.x + speed * dt
		end
		if GetKeyboardKey(KeyboardKey.D) then
			translateCam.x = translateCam.x - speed * dt
		end
		if GetKeyboardKey(KeyboardKey.Q) then
			translateCam.y = translateCam.y + speed * dt
		end
		if GetKeyboardKey(KeyboardKey.E) then
			translateCam.y = translateCam.y - speed * dt
		end

		--local cameraRotation = GetRotationInRadiansFromMatrix(entity:GetCamera().view)
		--print("rotation: x: ", degrees(cameraRotation.x), " y: ", degrees(cameraRotation.y), " z: ", degrees(cameraRotation.z))
		--local mouseDelta = GetMouseDelta()
		--print("mouse delta: x: ", mouseDelta.x, " y: ", mouseDelta.y)
		--cameraRotation.x = cameraRotation.x + mouseDelta.x * 0.5
		--cameraRotation.y = cameraRotation.y + mouseDelta.y * 0.5
		--print("\n")

		--entity:GetCamera().view = rotate(entity:GetCamera().view, cameraRotation.x, vec3.new(1, 0, 0))
		--entity:GetCamera().view = rotate(entity:GetCamera().view, cameraRotation.y, vec3.new(0, 1, 0))
		entity:GetCamera().view = translate(entity:GetCamera().view, translateCam)
	end
end

return Camera