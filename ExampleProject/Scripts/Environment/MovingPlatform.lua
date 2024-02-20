DEF_COMPONENT('MovingPlatform')
DEF_FIELD('MovingPlatform', 'v3Offset', Vec3.new())
DEF_FIELD('MovingPlatform', 'fSpeed', 5.0)

-- Engine Callbacks

function MovingPlatform:_OnCreate()
end

function MovingPlatform:_BeginPlay()
    local owner = self:Owner()
    self._v3InitialPosition = owner:Position()
    self._v3Destination = self._v3InitialPosition + self.v3Offset
    self._bMovingToDestination = true
end

function MovingPlatform:_Tick(time)
    local destination
    if self._bMovingToDestination then
        destination = self._v3Destination
    else
        destination = self._v3InitialPosition
    end

    local reachedDestination = self:MoveTowards(destination, time)
    if reachedDestination then
        self._bMovingToDestination = not self._bMovingToDestination
    end
end

-- Event Callbacks


-- Public API


-- Private Methods

function MovingPlatform:MoveTowards(destination, time)
    local owner = self:Owner()
    local ownerPosition = owner:Position()
    local difference = destination - ownerPosition
    if difference:Length() < 0.1 then
        return true
    end

    local direction = difference:Normalize()
    ownerPosition = ownerPosition + (direction * (time:DeltaTime() * self.fSpeed))
    owner:SetPosition(ownerPosition)
end
