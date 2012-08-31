function ENT:OnInitialize()
    
    self.SCUBE = StaticCube(math.floor(self:GetPos().x/64), math.floor(self:GetPos().y/64), math.floor(self:GetPos().z/64))
    self.SCUBE:SetSolid(true)
    self.statetime = 1
    self:ResetAnimOnPlay(true)
    self.open = false

end

function ENT:OnTick(dt)
    
    if self.statetimer then --toggles the door open/closed

        self.statetimer = self.statetimer + dt
        if self.statetimer > self.statetime then

            self.statetimer = nil
            if self.SCUBE:IsSolid() then --opening
                self:PlayAnim("open")
                self.SCUBE:SetSolid(false)
            else --closing
                self:PlayAnim("idle")
                self.SCUBE:SetSolid(true)
            end

        end

    end

end

function ENT:OnClick(ply)
    --self.SCUBE:SetSolid(not self.SCUBE:IsSolid())
    --local open = self.SCUBE:IsSolid()
    if self.open == false and self.statetimer == nil then
        self.open = true
        self:PlayAnim("opening")
        self.statetimer = 0
    elseif self.open == true and self.statetimer == nil then
        self.open = false
        self:PlayAnim("closing")
        self.statetimer = 0
        self.SCUBE:SetSolid(true)
    end
end

function ENT:OnRemove()
    self.SCUBE:Remove()
end