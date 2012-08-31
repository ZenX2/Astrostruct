local Closed = Animation("textures/door/door.png")
Closed.Name = "idle"
local Opening = Animation("textures/door/door.png", "textures/door/door_open001.png", "textures/door/door_open002.png", "textures/door/door_open003.png", "textures/door/door_open.png")
Opening.Name = "opening"
Opening.FPS = 5
local Open = Animation("textures/door/door_open.png")
Open.Name = "open"
local Closing = Animation("textures/door/door_open.png", "textures/door/door_open003.png", "textures/door/door_open002.png", "textures/door/door_open001.png", "textures/door/door.png")
Closing.Name = "closing"
Closing.FPS = 5
LoadTexture("door",Closed, Opening, Open, Closing)
