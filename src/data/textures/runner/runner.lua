Running = Animation(
	"data/textures/runner/runner_01.png",
	"data/textures/runner/runner_02.png",
	"data/textures/runner/runner_03.png",
	"data/textures/runner/runner_04.png",
	"data/textures/runner/runner_05.png",
	"data/textures/runner/runner_06.png"
)
Running.FPS = 20;
Running.Name = "run"

-- Then load them into the texture.
LoadTexture("runner",Running)
