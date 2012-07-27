TILE.Solid = false
TILE.Opaque = false

function TILE:OnInitialize()
    if (CLIENT) then
        self.MyLight = Light("point")
        self.MyLight:SetPos(self:GetPos()-Vector(0,0,TileSize/2))
        self.MyLight:SetRadius(1024)
        self.MyLight:SetColor(Color(0,1,0,1))
    end
end

function TILE:OnChange()
    if (CLIENT) then
        self.MyLight:Remove()
    end
end
