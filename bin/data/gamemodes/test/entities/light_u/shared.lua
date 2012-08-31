function ENT:OnInitialize()
    if (CLIENT) then
        self.MyLight = Light("point")
        self.MyLight:SetPos(self:GetPos())
        self.MyLight:SetRadius(WALLLIGHT_RADIUS)
        self.MyLight:SetColor(Color(1,1,1,1))
        --self.MyLight:SetColor(Color(math.random(),math.random(),math.random(),.5))
    end
end

function ENT:OnTick()
    if (CLIENT) then
        --self.MyLight:SetPos(self:GetPos())
        --self:SetPos(self:GetPos()+Vector(math.random()-math.random(),math.random()-math.random(),0))
    end
end

function ENT:OnClick(ply)
end

function ENT:OnRemove()
    if (CLIENT) then
        self.MyLight:Remove()
    end
end