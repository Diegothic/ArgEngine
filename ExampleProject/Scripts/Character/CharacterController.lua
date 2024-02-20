DEF_COMPONENT('CharacterController')

DEF_FIELD('CharacterController', "aCharacterAnimator", Actor.new())
DEF_FIELD('CharacterController', 'fMovementSpeedMax', 10.0)
DEF_FIELD('CharacterController', 'fAcceleration', 1.0)
DEF_FIELD('CharacterController', 'fDeacceleration', 1.0)
DEF_FIELD('CharacterController', 'fRotationSpeed', 10.0)
DEF_FIELD('CharacterController', 'aWeaponTrigger', Actor.new())

DEF_FIELD('CharacterController', 'SfxJump', Sound.new())
DEF_FIELD('CharacterController', 'SfxLand', Sound.new())
DEF_FIELD('CharacterController', 'SfxDodge', Sound.new())
DEF_FIELD('CharacterController', 'SfxStep', Sound.new())
DEF_FIELD('CharacterController', 'SfxDamaged', Sound.new())
DEF_FIELD('CharacterController', 'SfxDefeated', Sound.new())
DEF_FIELD('CharacterController', 'SfxSwordHit', Sound.new())
DEF_FIELD('CharacterController', 'SfxSwordSwoosh', Sound.new())

CharacterState = {
    Idle       = {},
    Moving     = {},
    Falling    = {},
    Attacking  = {},
    Dodgeing   = {},
    GettingHit = {},
    Defeated   = {}
}

-- Engine Callbacks

function CharacterController:_OnCreate()
    self._state = CharacterState.Idle
    self._fMovementSpeed = 0.0
    self._bGrounded = true
    self._cPhysicsBody = nil
    self._cCharacterAnimator = nil
    self._fTargetYaw = 0.0
    self._fGroundSlope = 0.0
    self._fAngleDiff = 0.0
    self._v3DesiredMovement = Vec3.new()

    self._iComboCounter = 1
    self._fComboCooldown = 0.0
    self._fComboTime = 0.0
    self._cWeaponTrigger = nil
    self._aDamagedActors = {}

    self._fDestructionTimer = 0.0
    self._bWaitForDestruction = false
end

function CharacterController:_OnDestroy()

end

function CharacterController:_BeginPlay()
    self._state = CharacterState.Idle
    self._fMovementSpeed = 0.0
    self._bGrounded = true

    local owner = self:Owner()
    self._cPhysicsBody = owner:GetComponent('PhysicsBody')
    if self._cPhysicsBody ~= nil then
        self._cPhysicsBody:AddOnCollisionListener(CharacterController._OnCollision, self)
    end

    self._cCharacterAnimator = self.aCharacterAnimator:GetComponent('CharacterAnimator')
    if self._cCharacterAnimator ~= nil then
        self._cCharacterAnimator:SetIdle()
    end

    self._fTargetYaw = 0.0
    self._fGroundSlope = 0.0
    self._fAngleDiff = 0.0
    self._v3DesiredMovement = Vec3.new()

    self._iComboCounter = 1
    self._fComboCooldown = 0.0
    self._fComboTime = 0.0
    self._cWeaponTrigger = self.aWeaponTrigger:GetComponent('TriggerVolume')
    if self._cWeaponTrigger ~= nil then
        self._cWeaponTrigger:AddOnEnterListener(CharacterController._OnWeaponTriggerEnter, self)
    end
    self._aDamagedActors = {}

    self._fDestructionTimer = 0.0
    self._bWaitForDestruction = false

    self._fStepTime = 0.4
end

function CharacterController:_Tick(time, input)
    if self._cPhysicsBody == nil then
        return
    end

    -- Check ground
    local ownerPosition = self:Owner():Position()
    local colliderHalfHeight = (self._cPhysicsBody:Size().X) + (self._cPhysicsBody:Size().Z * 0.5)
    local ignoreActors = { self:Owner() }
    local hitResult = self:World():Raycast(
        ownerPosition,
        Vec3.new(0.0, 0.0, -1.0),
        2.0,
        ignoreActors
    )

    if hitResult.HasHit then
        local hitDistance = (hitResult.HitPoint - ownerPosition):Length()
        if hitDistance > (colliderHalfHeight + 0.2)
        then
            self._bGrounded = false
        else
            local hitDotUp = hitResult.HitNormal:Dot(Vec3.new(0.0, 0.0, 1.0))
            if hitDotUp >= 0.0 then
                self._fGroundSlope = (1.0 - hitDotUp) * 90.0
                self._bGrounded = true
            end
        end
    else
        self._bGrounded = false
    end

    -- Update state
    if self._state == CharacterState.Idle then
        self:_UpdateIdle(time)
    elseif self._state == CharacterState.Moving then
        self:_UpdateMoving(time)
    elseif self._state == CharacterState.Falling then
        self:_UpdateFalling(time)
    elseif self._state == CharacterState.Attacking then
        self:_UpdateAttacking(time)
    elseif self._state == CharacterState.Dodgeing then
        self:_UpdateDodgeing(time)
    elseif self._state == CharacterState.GettingHit then
        self:_UpdateGettingHit(time)
    elseif self._state == CharacterState.Defeated then
        self:_UpdateDefeated(time)
    end

    if self._fComboTime > 0.0 then
        self._fComboTime = self._fComboTime - time:DeltaTime()
    end

    if self._fComboCooldown > 0.0 then
        self._fComboCooldown = self._fComboCooldown - time:DeltaTime()
    end

    -- Update rotation
    local owner = self:Owner()
    local ownerRotation = owner:Rotation()
    self._fAngleDiff = math.abs(self._fTargetYaw - ownerRotation.Z)
    if self._fAngleDiff > 180.0 then
        self._fAngleDiff = 360.0 - self._fAngleDiff
    end
    if self._fAngleDiff > 0.0 then
        local newRotation = Mathf.SLerp(
            ownerRotation,
            Vec3.new(0.0, 0.0, self._fTargetYaw),
            time:DeltaTime() * self.fRotationSpeed
        )
        owner:SetRotation(newRotation)
    end
end

function CharacterController:_OnDrawDebug(context)
    if self._cPhysicsBody == nil then
        return
    end

    local owner = self:Owner()
    local colliderHeight = self._cPhysicsBody:Size().Z
    local center = owner:Position() + (owner:Up() * (colliderHeight * 0.5))
    context:DrawLine(center, center + (owner:Forward() * 1.5), Vec3.new(1.0, 0.0, 0.0))

    local velocity = self._cPhysicsBody:Velocity()
    context:DrawLine(owner:Position(), owner:Position() + (velocity), Vec3.new(1.0, 1.0, 0.0))
end

-- Event Callbacks

function CharacterController:_OnCollision(collision)
    local hitDotUp = collision.HitNormal:Dot(Vec3.new(0.0, 0.0, 1.0))
    if hitDotUp >= 0.0 then
        self._bGrounded = true
    end
end

function CharacterController:_OnWeaponTriggerEnter(actor)
    if actor == self:Owner() then
        return
    end

    if self._state == CharacterState.Attacking
        and self._cCharacterAnimator:IsDamageEnabled()
    then
        for _, damagedActor in ipairs(self._aDamagedActors) do
            if damagedActor == actor then
                return
            end
        end

        table.insert(self._aDamagedActors, actor)
        local damageResponse = actor:GetComponent('DamageResponse')
        if damageResponse ~= nil then
            damageResponse:ApplyDamage(1.0)
            self._cCharacterAnimator:Pause(0.2)

            if self.SfxSwordHit:IsValid() then
                self:World():PlaySound3D(
                    self.SfxSwordHit,
                    self:Owner():Position(),
                    5.0,
                    false,
                    1.0,
                    0.1,
                    1.0,
                    0.2
                )
            end
        end
    end
end

-- Public API

function CharacterController:Move(v3Direction, fSpeed)
    if self._cPhysicsBody == nil then
        return
    end

    self._v3DesiredMovement = self._v3DesiredMovement + (v3Direction * fSpeed)
end

function CharacterController:Jump()
    if self._cPhysicsBody == nil then
        return
    end

    if not self._bGrounded then
        return
    end

    if self._state ~= CharacterState.Idle
        and self._state ~= CharacterState.Moving
    then
        return
    end

    self._state = CharacterState.Falling
    self._bGrounded = false
    local velocity = self._cPhysicsBody:Velocity()
    velocity = velocity + Vec3.new(0.0, 0.0, 12.0)
    self._cPhysicsBody:SetVelocity(velocity)

    if self.SfxJump:IsValid() then
        self:World():PlaySound3D(
            self.SfxJump,
            self:Owner():Position(),
            5.0,
            false,
            1.0,
            0.1,
            1.0,
            0.2
        )
    end
end

function CharacterController:Attack()
    if self._cPhysicsBody == nil then
        return
    end

    if not self._bGrounded
        or (self._state ~= CharacterState.Idle and self._state ~= CharacterState.Moving)
        or self._fComboCooldown > 0.0
    then
        return
    end

    self._aDamagedActors = {}
    self._cWeaponTrigger:Clear()

    if self._fComboTime > 0.0 then
        self._fComboTime = 0.0
        self._iComboCounter = self._iComboCounter + 1
    else
        self._iComboCounter = 1
    end

    self._state = CharacterState.Attacking

    local owner = self:Owner()
    local targetRotation = Vec3.new(0.0, 0.0, self._fTargetYaw)
    owner:SetRotation(targetRotation)

    local ownerForward = owner:Forward()

    local velocity = self._cPhysicsBody:Velocity()
    velocity.X = ownerForward.X * 2.0;
    velocity.Y = ownerForward.Y * 2.0;
    self._cPhysicsBody:SetVelocity(velocity)

    self._cCharacterAnimator:Attack(self._iComboCounter)

    if self.SfxSwordSwoosh:IsValid() then
        self:World():PlaySound3D(
            self.SfxSwordSwoosh,
            self:Owner():Position(),
            5.0,
            false,
            1.0,
            0.1,
            1.0,
            0.2
        )
    end
end

function CharacterController:Dodge()
    if self._cPhysicsBody == nil then
        return
    end

    if not self._bGrounded
        or (self._state ~= CharacterState.Moving
            and self._state ~= CharacterState.Idle)
    then
        return
    end

    self._state = CharacterState.Dodgeing

    local owner = self:Owner()
    local targetRotation = Vec3.new(0.0, 0.0, self._fTargetYaw)
    owner:SetRotation(targetRotation)

    local ownerForward = owner:Forward()

    local velocity = self._cPhysicsBody:Velocity()
    velocity.X = ownerForward.X * 9.0;
    velocity.Y = ownerForward.Y * 9.0;
    self._cPhysicsBody:SetVelocity(velocity)

    self._cCharacterAnimator:Dodge()

    if self.SfxDodge:IsValid() then
        self:World():PlaySound3D(
            self.SfxDodge,
            self:Owner():Position(),
            5.0,
            false,
            1.0,
            0.1,
            1.0,
            0.2
        )
    end
end

function CharacterController:GetHit()
    if self._cPhysicsBody == nil then
        return
    end

    self._state = CharacterState.GettingHit

    local owner = self:Owner()
    local ownerForward = owner:Forward()

    local velocity = self._cPhysicsBody:Velocity()
    velocity.X = ownerForward.X * -3.0;
    velocity.Y = ownerForward.Y * -3.0;
    self._cPhysicsBody:SetVelocity(velocity)

    self._cCharacterAnimator:GetHit()
    self._cCharacterAnimator:Pause(0.2)

    if self.SfxDamaged:IsValid() then
        self:World():PlaySound3D(
            self.SfxDamaged,
            self:Owner():Position(),
            5.0,
            false,
            1.0,
            0.1,
            1.0,
            0.2
        )
    end
end

function CharacterController:GetDefeated()
    if self._cPhysicsBody == nil then
        return
    end

    self._state = CharacterState.Defeated

    self._cPhysicsBody:SetVelocity(Vec3.new(0.0))
    self._cPhysicsBody:SetIsDynamic(false)

    self._cCharacterAnimator:GetDefeated()
    self._cCharacterAnimator:Pause(0.2)

    if self.SfxDefeated:IsValid() then
        self:World():PlaySound3D(
            self.SfxDefeated,
            self:Owner():Position(),
            5.0,
            false,
            1.0,
            0.1,
            1.0,
            0.2
        )
    end
end

-- Private Methods

function CharacterController:_LookAtDirection(v3Direction)
    self._fTargetYaw = math.deg(math.atan(v3Direction.Y, v3Direction.X))
    local owner = self:Owner()
    local ownerRotation = owner:Rotation()
    self._fAngleDiff = math.abs(self._fTargetYaw - ownerRotation.Z)
    if self._fAngleDiff > 180.0 then
        self._fAngleDiff = 360.0 - self._fAngleDiff
    end
end

function CharacterController:_Move(v3Direction, fSpeed)
    local velocity = self._cPhysicsBody:Velocity()
    local velocityFlat = Vec3.new(velocity.X, velocity.Y, 0.0)

    local desiredMovement = v3Direction * (fSpeed * self.fAcceleration)
    local newVelocity = velocityFlat + desiredMovement
    local velocityDirection = newVelocity:Normalize()
    local velocityLength = newVelocity:Length()
    velocityLength = Mathf.Clamp(velocityLength, 0.5, self.fMovementSpeedMax)
    newVelocity = velocityDirection * velocityLength
    newVelocity.Z = velocity.Z

    self._cPhysicsBody:SetVelocity(newVelocity)
end

function CharacterController:_UpdateIdle(time)
    if not self._bGrounded then
        self._state = CharacterState.Falling
        return
    end

    self._fMovementSpeed = 0.0
    local desiredSpeed = self._v3DesiredMovement:Length()
    if desiredSpeed > 0.0 then
        local desiredDirection = self._v3DesiredMovement:Normalize()
        self:_LookAtDirection(desiredDirection)
        if self._fAngleDiff <= 30.0 then
            self._state = CharacterState.Moving
        end

        self._v3DesiredMovement = Vec3.new(0.0)
    elseif self._bGrounded and self._fGroundSlope < 60.0 then
        self._cPhysicsBody:SetVelocity(Vec3.new(0.0))
    end

    self._cCharacterAnimator:SetIdle()
end

function CharacterController:_UpdateMoving(time)
    if not self._bGrounded then
        self._state = CharacterState.Falling
        return
    end

    local velocity = self._cPhysicsBody:Velocity()
    local velocityFlat = Vec3.new(velocity.X, velocity.Y, 0.0)
    local desiredSpeed = self._v3DesiredMovement:Length()
    if desiredSpeed > 0.0 then
        local desiredDirection = self._v3DesiredMovement:Normalize()
        self:_LookAtDirection(desiredDirection)
        if self._fAngleDiff <= 60.0 then
            local desiredMovement = desiredDirection * (desiredSpeed * time:DeltaTime() * self.fAcceleration)

            local newVelocity = velocityFlat + desiredMovement
            local velocityLength = newVelocity:Length()
            if velocityLength > 0.0 then
                local velocityDirection = newVelocity:Normalize()
                velocityLength = Mathf.Clamp(velocityLength, 0.5, self.fMovementSpeedMax)
                newVelocity = velocityDirection * velocityLength
                newVelocity.Z = velocity.Z - 0.5

                self._cPhysicsBody:SetVelocity(newVelocity)
                self._fMovementSpeed = velocityLength
            end
        end
    else
        local velocityLength = velocityFlat:Length()
        if velocityLength > 0.0 then
            local velocityNormal = velocityFlat:Normalize()
            velocityLength = velocityLength - (time:DeltaTime() * self.fDeacceleration)
            velocityLength = math.max(0.0, velocityLength)
            local newVelocity = velocityNormal * velocityLength
            newVelocity.Z = velocity.Z - 0.5

            self._cPhysicsBody:SetVelocity(newVelocity)
            self._fMovementSpeed = velocityLength
        end
    end

    self._v3DesiredMovement = Vec3.new(0.0)

    if self._fMovementSpeed <= 0.0 then
        self._state = CharacterState.Idle
        return
    end

    self._cCharacterAnimator:SetMoving(self._fMovementSpeed)

    if self._fStepTime > 0.0 then
        self._fStepTime = self._fStepTime - time:DeltaTime()
    else
        self._fStepTime = 0.5
        if self.SfxStep:IsValid() then
            self:World():PlaySound3D(
                self.SfxStep,
                self:Owner():Position(),
                5.0,
                false,
                1.0,
                0.1,
                0.8,
                0.2
            )
        end
    end
end

function CharacterController:_UpdateFalling(time)
    if self._bGrounded then
        self._state = CharacterState.Moving

        if self.SfxLand:IsValid() then
            self:World():PlaySound3D(
                self.SfxLand,
                self:Owner():Position(),
                5.0,
                false,
                1.0,
                0.1,
                1.0,
                0.2
            )
        end

        return
    end

    local velocity = self._cPhysicsBody:Velocity()
    local velocityFlat = Vec3.new(velocity.X, velocity.Y, 0.0)
    local desiredSpeed = self._v3DesiredMovement:Length()
    if desiredSpeed > 0.0 then
        local desiredDirection = self._v3DesiredMovement:Normalize()
        self:_LookAtDirection(desiredDirection)
        local desiredMovement = desiredDirection
            * (desiredSpeed * time:DeltaTime()
                * (self.fAcceleration * 0.2))

        local newVelocity = velocityFlat + desiredMovement
        local velocityLength = newVelocity:Length()
        if velocityLength > 0.0 then
            local velocityDirection = newVelocity:Normalize()
            velocityLength = Mathf.Clamp(velocityLength, 0.0, self.fMovementSpeedMax)
            newVelocity = velocityDirection * velocityLength
            newVelocity.Z = velocity.Z

            self._cPhysicsBody:SetVelocity(newVelocity)
        end
    end

    local velocity = self._cPhysicsBody:Velocity()
    if velocity.Z < 0.0 then
        velocity.Z = velocity.Z - time:DeltaTime() * 15.0
    end
    self._cPhysicsBody:SetVelocity(velocity)

    self._fMovementSpeed = 0.0
    self._v3DesiredMovement = Vec3.new(0.0)

    self._cCharacterAnimator:SetFalling()
end

function CharacterController:_UpdateAttacking(time)
    if not self._bGrounded then
        self._state = CharacterState.Falling
        return
    end

    if self._cCharacterAnimator:HasAnimationEnded() then
        self._state = CharacterState.Moving
        if self._iComboCounter >= 3 then
            self._iComboCounter = 1
            self._fComboCooldown = 0.3
        else
            self._fComboTime = 0.3
        end
        return
    end
end

function CharacterController:_UpdateDodgeing(time)
    if not self._bGrounded then
        self._state = CharacterState.Falling
        return
    end

    if self._cCharacterAnimator:HasAnimationEnded() then
        self._state = CharacterState.Moving
        return
    end
end

function CharacterController:_UpdateGettingHit(time)
    if self._cCharacterAnimator:HasAnimationEnded() then
        self._state = CharacterState.Moving
        return
    end
end

function CharacterController:_UpdateDefeated(time)
    if self._cCharacterAnimator:HasAnimationEnded() then
        self._fDestructionTimer = 1.0
        self._bWaitForDestruction = true
        self._cCharacterAnimator:PlayDefeatedIdle()
        return
    end

    if self._bWaitForDestruction then
        if self._fDestructionTimer > 0.0 then
            self._fDestructionTimer = self._fDestructionTimer - time:DeltaTime()
        else
            self:Owner():Destroy()
        end
    end
end
