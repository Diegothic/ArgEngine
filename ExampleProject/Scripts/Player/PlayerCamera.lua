DEF_COMPONENT('PlayerCamera')
DEF_FIELD('PlayerCamera', 'aCameraTarget', Actor.new())
DEF_FIELD('PlayerCamera', 'aPlayer', Actor.new())
DEF_FIELD('PlayerCamera', 'v3TargetOffset', Vec3.new())
DEF_FIELD('PlayerCamera', 'fTargetMaxDistance', 5.0)
DEF_FIELD('PlayerCamera', 'fFollowSpeed', 1.0)
DEF_FIELD('PlayerCamera', 'fPitchMin', -80.0)
DEF_FIELD('PlayerCamera', 'fPitchMax', 80.0)

function PlayerCamera:_OnCreate()
    self._fPitch = 0.0
    self._fYaw = 0.0
end

function PlayerCamera:_Tick(time, input)
    local mouseDelta = Vec2.new(input:MouseDelta()) * 1.0
    self._fYaw = self._fYaw - (mouseDelta.X * time:DeltaTime() * 10.0)
    self._fPitch = self._fPitch + (mouseDelta.Y * time:DeltaTime() * 10.0)

    self._fPitch = Mathf.Clamp(self._fPitch, self.fPitchMin, self.fPitchMax)

    self.aCameraTarget:SetRotation(Vec3.new(0.0, self._fPitch, self._fYaw))

    local owner = self:Owner()
    local targetPosition = self.aCameraTarget:Position()
    local lookTarget = targetPosition + self.v3TargetOffset
    targetPosition = lookTarget - (self.aCameraTarget:Forward() * self.fTargetMaxDistance)

    local ignoreActors = { self.aPlayer }
    local hitResult = self:World():Raycast(
        lookTarget,
        (targetPosition - lookTarget):Normalize(),
        self.fTargetMaxDistance,
        ignoreActors
    )

    if hitResult.HasHit then
        targetPosition = Vec3.new(hitResult.HitPoint)
    end

    owner:SetPosition(Mathf.Lerp(owner:Position(), targetPosition, time:DeltaTime() * self.fFollowSpeed))

    local lookTargetDiff = lookTarget - owner:Position()
    local lookRotation = Mathf.LookAtRotation(lookTargetDiff)
    owner:SetRotation(lookRotation)
end
