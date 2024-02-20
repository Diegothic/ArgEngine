DEF_COMPONENT('DamageResponse')

-- Engine Callbacks

function DamageResponse:_OnCreate()
    self.OnDamage = Event.new()
end

function DamageResponse:_OnDestroy()

end

function DamageResponse:_BeginPlay()
end

function DamageResponse:_Tick(time, input)

end

function DamageResponse:_OnDrawDebug(context)

end

-- Event Callbacks

-- Public API

function DamageResponse:ApplyDamage(fDamage)
    self.OnDamage:Invoke({ fDamage = fDamage })
end

-- Private Methods
