DEF_COMPONENT('EnemyController')
DEF_FIELD('EnemyController', 'fSightRange', 5.0)

-- Engine Callbacks

function EnemyController:_OnCreate()
    self._cCharacterController = nil
    self._cCharacterStats = nil
    self._bDefeated = false
    self._aTarget = Actor.new()
end

function EnemyController:_OnDestroy()

end

function EnemyController:_BeginPlay()
    local owner = self:Owner()
    self._cCharacterController = owner:GetComponent('CharacterController')
    self._cCharacterStats = owner:GetComponent('CharacterStats')
    if self._cCharacterStats ~= nil then
        self._cCharacterStats.OnDefeated:AddListener(EnemyController._OnDefeated, self)
    end

    self._bDefeated = false
end

function EnemyController:_Tick(time, input)
    if self._cCharacterController == nil then
        return
    end

    if self._bDefeated then
        return
    end

    if self._aTarget:IsValid() then
        local owner = self:Owner()
        local ownerPosition = owner:Position()
        local targetPosition = self._aTarget:Position()
        local difference = targetPosition - ownerPosition
        local distanceToTarget = difference:Length()
        if distanceToTarget > 1.5 then
            local desiredDirection = difference:Normalize()
            self._cCharacterController:Move(desiredDirection, 0.6)
        else
            self._cCharacterController:Attack()
        end
    else
        local owner = self:Owner()
        local ownerPosition = owner:Position()
        local world = self:World()
        local aNerbyActors = world:CheckSphere(ownerPosition, self.fSightRange)
        for k = 1, #aNerbyActors do
            local aNerbyActor = aNerbyActors[k]
            if aNerbyActor:GetComponent('PlayerController') ~= nil then
                self._aTarget = Actor.new(aNerbyActor)
                break
            end
        end
    end
end

function EnemyController:_OnDrawDebug(context)
    local owner = self:Owner()
    local ownerPosition = owner:Position()
    context:DrawSphere(
        ownerPosition,
        Vec3.new(0.0),
        self.fSightRange,
        Vec3.new(0.0, 1.0, 1.0)
    )
end

-- Event Callbacks

function EnemyController:_OnDefeated()
    self._bDefeated = true
end

-- Public API


-- Private Methods
