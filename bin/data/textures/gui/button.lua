local Idle = Animation("textures/gui/button_idle.png")
Idle.Name = "idle"
Idle.BorderSize = 1
local Active = Animation("textures/gui/button_active.png")
Active.Name = "active"
Active.BorderSize = 1
local Pressed = Animation("textures/gui/button_pressed.png")
Pressed.Name = "pressed"
Pressed.BorderSize = 1

LoadTexture("button",Idle,Active,Pressed);
