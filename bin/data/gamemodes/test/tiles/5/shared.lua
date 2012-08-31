TILE.Solid = false
TILE.Opaque = false

function TILE:OnInitialize()
    if (CLIENT) then
        self.MyLight = Light("point")
        self.MyLight:SetPos(self:GetPos()-Vector(0,0,TileSize/2))
        self.MyLight:SetRadius(4096)
        self.MyLight:SetColor(Color(1,1,1,1))
    end
end

function TILE:OnTick()

    if (CLIENT) then

        self.MyLight:SetColor(Color(math.random(),math.random(),math.random(),math.random()))

    end

end

function TILE:OnRemove()
    if (CLIENT) then
        self.MyLight:Remove()
    end
end
