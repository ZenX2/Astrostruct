-- The game plays a random animation when it spawns a star.
local AStar = Animation( "textures/space/star_001.png" )
AStar.Name = "idle"
local BStar = Animation("textures/space/star_002.png" )
BStar.Name = "star1"

LoadTexture("star",AStar,BStar)
