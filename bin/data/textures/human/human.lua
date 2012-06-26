local Idle = Animation(	"textures/human/human_idle000.png",
					"textures/human/human_idle001.png")
Idle.Name = "idle"
Idle.FPS = 1
local Running = Animation("textures/human/human_walk000.png",
					"textures/human/human_walk001.png")
Running.Name = "run"
Running.FPS = 5

LoadTexture("human",Idle,Running)
