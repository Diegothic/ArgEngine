DEF_COMPONENT('PlayerController')

-- Engine Callbacks

function PlayerController:_OnCreate()
    self._cCharacterController = nil
    self._cCharacterStats = nil
end

function PlayerController:_OnDestroy()

end

function PlayerController:_BeginPlay()
    local owner = self:Owner()
    self._cCharacterController = owner:GetComponent('CharacterController')
    self._cCharacterStats = owner:GetComponent('CharacterStats')
    if self._cCharacterStats ~= nil then
        self._cCharacterStats.OnDefeated:AddListener(PlayerController._OnDefeated, self)
    end
end

function PlayerController:_Tick(time, input)
    if self._cCharacterController == nil then
        return
    end

    local desiredMovement = self:_FindDesiredMovement(input)
    local desiredSpeed = desiredMovement:Length()
    if desiredSpeed > 0.0 then
        local desiredDirection = desiredMovement:Normalize()
        self._cCharacterController:Move(desiredDirection, desiredSpeed)
    end

    if input:KeyPressed(KeyCode.Space) then
        self._cCharacterController:Jump()
    end

    if input:MouseButtonPressed(MouseButton.Left) then
        self._cCharacterController:Attack()
    end

    if input:MouseButtonPressed(MouseButton.Right) then
        self._cCharacterController:Dodge()
    end

    if input:KeyPressed(KeyCode.H) then
        self._cCharacterController:GetHit()
    end

    if input:KeyPressed(KeyCode.R) then
        self:_OnDefeated()
    end
end

function PlayerController:_OnDrawDebug(context)
    
end

-- Event Callbacks

function PlayerController:_OnDefeated()
    local world = self:World()
    world:Load('Maps\\LevelOne')
end

-- Public API


-- Private Methods

function PlayerController:_FindDesiredMovement (input)
    local camera = self:World():MainCamera()
    local cameraActor = camera:Owner()
    local cameraForward = cameraActor:Forward()
    cameraForward.Z = 0.0
    cameraForward = cameraForward:Normalize()
    local cameraRight = cameraActor:Right()

    local desiredMovement = Vec3.new(0.0)
    if input:KeyDown(KeyCode.W) then
        desiredMovement = desiredMovement + cameraForward
    end

    if input:KeyDown(KeyCode.S) then
        desiredMovement = desiredMovement - cameraForward
    end

    if input:KeyDown(KeyCode.A) then
        desiredMovement = desiredMovement - cameraRight
    end

    if input:KeyDown(KeyCode.D) then
        desiredMovement = desiredMovement + cameraRight
    end

    return desiredMovement
end
