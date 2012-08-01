MapSkin = "grimy"
MaxTiles = 4
function GAME:OnInitialize()
    print("Running test gamemode!")
end

function GAME:OnPlayerSpawn(Player)
    local SpawnArray,ArrayLength = GetEntitiesByName("spawnpoint")
    if (ArrayLength ~= 0) then
        local SpawnID = math.floor(math.random()*ArrayLength)+1
        Player:SetPos(SpawnArray[SpawnID]:GetPos())
    end
end
