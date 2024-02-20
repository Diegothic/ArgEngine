DEF_COMPONENT('CharacterStats')
DEF_FIELD('CharacterStats', 'fMaxHealth', 10.0)

-- Engine Callbacks

function CharacterStats:_OnCreate()
    self._cCharacterController = nil
    self._cDamageResponse = nil

    self._fCurrentHealth = 10.0
    self._bDefeated = false

    self.OnDefeated = Event.new()
end

function CharacterStats:_OnDestroy()

end

function CharacterStats:_BeginPlay()
    local owner = self:Owner()
    self._cCharacterController = owner:GetComponent('CharacterController')
    self._cDamageResponse = owner:GetComponent('DamageResponse')
    if self._cDamageResponse ~= nil then
        self._cDamageResponse.OnDamage:AddListener(CharacterStats._OnDamage, self)
    end

    self._fCurrentHealth = self.fMaxHealth
    self._bDefeated = false
end

function CharacterStats:_Tick(time, input)

end

function CharacterStats:_OnDrawDebug(context)
    local owner = self:Owner()
    local ownerPosition = owner:Position()
    local indicatorPosition = ownerPosition + (owner:Up() * 1.5)
    context:DrawSphere(
        indicatorPosition,
        Vec3.new(0.0),
        self._fCurrentHealth / 20.0,
        Vec3.new(1.0, 0.0, 0.0)
    )
end

-- Event Callbacks

function CharacterStats:_OnDamage(damageData)
    self._fCurrentHealth = math.max(0.0, self._fCurrentHealth - damageData.fDamage)
    if self._cCharacterController == nil then
        return
    end

    if self._bDefeated then
        return
    end

    if self._fCurrentHealth <= 0.0 then
        self._bDefeated = true
        self._cCharacterController:GetDefeated()
        self.OnDefeated:Invoke({})
    else
        self._cCharacterController:GetHit()
    end
end

-- Public API


-- Private Methods
