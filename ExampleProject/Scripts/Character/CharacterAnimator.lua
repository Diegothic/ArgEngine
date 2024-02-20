DEF_COMPONENT('CharacterAnimator')

DEF_FIELD('CharacterAnimator', 'aSkeletalModel', Actor.new())

DEF_FIELD('CharacterAnimator', 'IdleAnim', SkeletalAnimation.new())
DEF_FIELD('CharacterAnimator', 'WalkAnim', SkeletalAnimation.new())
DEF_FIELD('CharacterAnimator', 'JogAnim', SkeletalAnimation.new())
DEF_FIELD('CharacterAnimator', 'FallAnim', SkeletalAnimation.new())

DEF_FIELD('CharacterAnimator', 'AttackAnim1', SkeletalAnimation.new())
DEF_FIELD('CharacterAnimator', 'AttackAnim2', SkeletalAnimation.new())
DEF_FIELD('CharacterAnimator', 'AttackAnim3', SkeletalAnimation.new())

DEF_FIELD('CharacterAnimator', 'GotHitAnim', SkeletalAnimation.new())
DEF_FIELD('CharacterAnimator', 'DodgeAnim', SkeletalAnimation.new())
DEF_FIELD('CharacterAnimator', 'DefeatedAnim', SkeletalAnimation.new())
DEF_FIELD('CharacterAnimator', 'DefeatedIdleAnim', SkeletalAnimation.new())

-- Engine Callbacks

function CharacterAnimator:_OnCreate()
    self._cSkeletalModel = nil
    self._bAnimationEnded = true;
    self._attackAnims = nil
    self._fPauseTime = 0.0
    self._bPaused = false

    self._bDamage = false
end

function CharacterAnimator:_OnDestroy()

end

function CharacterAnimator:_BeginPlay()
    self._bAnimationEnded = true;
    self._cSkeletalModel = self.aSkeletalModel:GetComponent('SkeletalModel')
    if self._cSkeletalModel ~= nil then
        self._cSkeletalModel:AddOnAnimationEndListener(CharacterAnimator._OnAnimationEnded, self)
        self._cSkeletalModel:AddOnAnimationEventListener(CharacterAnimator._OnAnimationEvent, self)
    end

    self._attackAnims = {
        self.AttackAnim1,
        self.AttackAnim2,
        self.AttackAnim3
    }
    self._fPauseTime = 0.0
    self._bPaused = false

    self._bDamage = false
end

function CharacterAnimator:_Tick(time, input)
    if self._bPaused then
        if self._fPauseTime > 0.0 then
            self._fPauseTime = self._fPauseTime - time:DeltaTime()
        else
            self._bPaused = false
            self._cSkeletalModel:Unpause()
        end
    end
end

function CharacterAnimator:_OnDrawDebug(context)

end

-- Event Callbacks

function CharacterAnimator:_OnAnimationEnded()
    self._bAnimationEnded = true
end

function CharacterAnimator:_OnAnimationEvent(event)
    if event == 'BeginDamage' then
        self._bDamage = true
    elseif event == 'EndDamage' then
        self._bDamage = false
    end
end

-- Public API

function CharacterAnimator:HasAnimationEnded()
    return self._bAnimationEnded
end

function CharacterAnimator:IsDamageEnabled()
    return self._bDamage
end

function CharacterAnimator:SetIdle()
    if self._cSkeletalModel == nil then
        return
    end

    if self._cSkeletalModel:CurrentAnimation() ~= self.IdleAnim then
        self._cSkeletalModel:Play(self.IdleAnim, true)
    end
end

function CharacterAnimator:SetMoving(movementSpeed)
    if self._cSkeletalModel == nil then
        return
    end

    if movementSpeed <= 5.0 then
        if self._cSkeletalModel:CurrentAnimation() ~= self.WalkAnim then
            self._cSkeletalModel:Play(self.WalkAnim, true)
        end
    else
        if self._cSkeletalModel:CurrentAnimation() ~= self.JogAnim then
            self._cSkeletalModel:Play(self.JogAnim, true)
        end
    end
end

function CharacterAnimator:SetFalling()
    if self._cSkeletalModel == nil then
        return
    end

    if self._cSkeletalModel:CurrentAnimation() ~= self.FallAnim then
        self._cSkeletalModel:Play(self.FallAnim, true)
    end
end

function CharacterAnimator:Attack(iNum)
    if self._cSkeletalModel == nil then
        return
    end

    self._bAnimationEnded = false
    self._bDamage = false
    self._cSkeletalModel:Play(self._attackAnims[iNum])
end

function CharacterAnimator:Dodge()
    if self._cSkeletalModel == nil then
        return
    end

    self._bAnimationEnded = false
    self._cSkeletalModel:Play(self.DodgeAnim)
end

function CharacterAnimator:GetHit()
    if self._cSkeletalModel == nil then
        return
    end

    self._bAnimationEnded = false
    self._cSkeletalModel:Play(self.GotHitAnim)
end

function CharacterAnimator:GetDefeated()
    if self._cSkeletalModel == nil then
        return
    end

    self._bAnimationEnded = false
    self._cSkeletalModel:Play(self.DefeatedAnim)
end

function CharacterAnimator:PlayDefeatedIdle()
    if self._cSkeletalModel == nil then
        return
    end

    self._bAnimationEnded = false
    if self._cSkeletalModel:CurrentAnimation() ~= self.DefeatedIdleAnim then
        self._cSkeletalModel:Play(self.DefeatedIdleAnim)
    end
end

function CharacterAnimator:Pause(fTime)
    if self._cSkeletalModel == nil then
        return
    end

    self._bPaused = true
    self._fPauseTime = fTime
    self._cSkeletalModel:Pause()
end

-- Private Methods
