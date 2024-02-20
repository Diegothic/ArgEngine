DEF_COMPONENT('MovingDoors')
DEF_FIELD('MovingDoors', 'aTriggerVolumeActor', Actor.new())
DEF_FIELD('MovingDoors', 'aTriggerActor', Actor.new())
DEF_FIELD('MovingDoors', 'fHeight', 2.0)
DEF_FIELD('MovingDoors', 'fSpeed', 5.0)

-- Engine Callbacks

function MovingDoors:_OnCreate()
    self._cTrigger = nil
    self._cSoundPlayer = nil

    self._bMovingUp = false
end

function MovingDoors:_BeginPlay()
    local owner = self:Owner()
    self._fInitialHeight = owner:Position().Z
    self._fUpHeight = self._fInitialHeight + self.fHeight

    if self.aTriggerVolumeActor:IsValid() then
        self._cTrigger = self.aTriggerVolumeActor:GetComponent('TriggerVolume')
        if self._cTrigger ~= nil then
            self._cTrigger:AddOnEnterListener(MovingDoors._OnTriggerEnter, self)
            self._cTrigger:AddOnExitListener(MovingDoors._OnTriggerExit, self)
        end
    end

    self._cSoundPlayer = owner:GetComponent('SoundPlayer')
    if self._cSoundPlayer ~= nil then
        self._cSoundPlayer:Play()
        self._cSoundPlayer:Pause()
    end
end

function MovingDoors:_Tick(time)
    local owner = self:Owner()
    local ownerPosition = owner:Position()
    if self._bMovingUp then
        if ownerPosition.Z < self._fUpHeight then
            ownerPosition.Z = ownerPosition.Z + (time:DeltaTime() * self.fSpeed)
            if ownerPosition.Z >= self._fUpHeight and self._cSoundPlayer ~= nil then
                self._cSoundPlayer:Pause()
            end
            ownerPosition.Z = math.min(ownerPosition.Z, self._fUpHeight)
            owner:SetPosition(ownerPosition)
        end
    else
        if ownerPosition.Z > self._fInitialHeight then
            ownerPosition.Z = ownerPosition.Z - (time:DeltaTime() * self.fSpeed)
            if ownerPosition.Z <= self._fInitialHeight and self._cSoundPlayer ~= nil then
                self._cSoundPlayer:Pause()
            end
            ownerPosition.Z = math.max(ownerPosition.Z, self._fInitialHeight)
            owner:SetPosition(ownerPosition)
        end
    end
end

-- Event Callbacks

function MovingDoors:_OnTriggerEnter(actor)
    if not self.aTriggerActor:IsValid() then
        return
    end

    if actor == self.aTriggerActor then
        self:MoveUp()
    end
end

function MovingDoors:_OnTriggerExit(actor)
    if not self.aTriggerActor:IsValid() then
        return
    end

    if actor == self.aTriggerActor then
        self:MoveDown()
    end
end

-- Public API

function MovingDoors:MoveUp()
    self._bMovingUp = true
    if self._cSoundPlayer ~= nil then
        self._cSoundPlayer:Unpause()
    end
end

function MovingDoors:MoveDown()
    self._bMovingUp = false
    if self._cSoundPlayer ~= nil then
        self._cSoundPlayer:Unpause()
    end
end

-- Private Methods
