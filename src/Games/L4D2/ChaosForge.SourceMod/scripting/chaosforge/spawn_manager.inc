#if defined _chaosforge_spawn_manager_included
    #endinput
#endif
#define _chaosforge_spawn_manager_included

#define CHAOSFORGE_TEAM_SURVIVOR 2

bool ChaosTarget_GetCrosshairPosition(
    int preferredClient,
    float position[3],
    int &resolvedClient,
    int &hitEntity
)
{
    resolvedClient = ChaosSpawn_GetAvailableSurvivor(preferredClient);
    hitEntity = -1;

    if (resolvedClient == 0)
    {
        return false;
    }

    float eyePosition[3];
    float eyeAngles[3];

    GetClientEyePosition(resolvedClient, eyePosition);
    GetClientEyeAngles(resolvedClient, eyeAngles);

    Handle trace = TR_TraceRayFilterEx(
        eyePosition,
        eyeAngles,
        MASK_SHOT,
        RayType_Infinite,
        ChaosTarget_TraceFilter,
        resolvedClient
    );

    if (trace == INVALID_HANDLE)
    {
        return false;
    }

    bool didHit = TR_DidHit(trace);

    if (didHit)
    {
        TR_GetEndPosition(position, trace);
        hitEntity = TR_GetEntityIndex(trace);
    }

    delete trace;
    return didHit;
}

public bool ChaosTarget_TraceFilter(
    int entity,
    int contentsMask,
    any ignoredClient
)
{
    return entity != ignoredClient;
}

bool ChaosSpawn_HunterAtCrosshair(
    int preferredClient,
    int &spawnedEntity,
    int &resolvedClient,
    float position[3]
)
{
    spawnedEntity = -1;

    int hitEntity;

    if (!ChaosTarget_GetCrosshairPosition(
        preferredClient,
        position,
        resolvedClient,
        hitEntity
    ))
    {
        PrintToServer(
            "[ChaosForge] Hunter crosshair spawn failed: no valid crosshair target."
        );

        return false;
    }

    float angles[3];
    GetClientEyeAngles(resolvedClient, angles);

    angles[0] = 0.0;
    angles[2] = 0.0;

    spawnedEntity = L4D2_SpawnSpecial(
        L4D2_ZOMBIE_CLASS_HUNTER,
        position,
        angles
    );

    if (spawnedEntity <= 0)
    {
        return false;
    }

    PrintToServer(
        "[ChaosForge] Hunter entity %d spawned for Survivor %N at X %.2f, Y %.2f, Z %.2f (trace hit entity %d).",
        spawnedEntity,
        resolvedClient,
        position[0],
        position[1],
        position[2],
        hitEntity
    );

    return true;
}

int ChaosSpawn_GetAvailableSurvivor(int preferredClient = 0)
{
    if (ChaosSpawn_IsLivingSurvivor(preferredClient))
    {
        return preferredClient;
    }

    for (int client = 1; client <= MaxClients; client++)
    {
        if (ChaosSpawn_IsLivingSurvivor(client) && !IsFakeClient(client))
        {
            return client;
        }
    }

    for (int client = 1; client <= MaxClients; client++)
    {
        if (ChaosSpawn_IsLivingSurvivor(client))
        {
            return client;
        }
    }

    return 0;
}

bool ChaosSpawn_IsLivingSurvivor(int client)
{
    return client > 0
        && client <= MaxClients
        && IsClientInGame(client)
        && IsPlayerAlive(client)
        && GetClientTeam(client) == CHAOSFORGE_TEAM_SURVIVOR;
}

int ChaosSpawn_SpecialInfectedEntity(
    const char[] infectedName,
    int preferredClient = 0
)
{
    float position[3];
    float angles[3];

    int resolvedClient;
    int hitEntity;

    if (!ChaosTarget_GetCrosshairPosition(
        preferredClient,
        position,
        resolvedClient,
        hitEntity
    ))
    {
        PrintToServer(
            "[ChaosForge] %s spawn failed: no valid Survivor crosshair target.",
            infectedName
        );

        return -1;
    }

    GetClientEyeAngles(resolvedClient, angles);
    angles[0] = 0.0;
    angles[2] = 0.0;

    int spawnedEntity = -1;

    if (StrEqual(infectedName, "hunter", false))
    {
        spawnedEntity = L4D2_SpawnSpecial(L4D2_ZOMBIE_CLASS_HUNTER, position, angles);
    }
    else if (StrEqual(infectedName, "smoker", false))
    {
        spawnedEntity = L4D2_SpawnSpecial(L4D2_ZOMBIE_CLASS_SMOKER, position, angles);
    }
    else if (StrEqual(infectedName, "boomer", false))
    {
        spawnedEntity = L4D2_SpawnSpecial(L4D2_ZOMBIE_CLASS_BOOMER, position, angles);
    }
    else if (StrEqual(infectedName, "spitter", false))
    {
        spawnedEntity = L4D2_SpawnSpecial(L4D2_ZOMBIE_CLASS_SPITTER, position, angles);
    }
    else if (StrEqual(infectedName, "jockey", false))
    {
        spawnedEntity = L4D2_SpawnSpecial(L4D2_ZOMBIE_CLASS_JOCKEY, position, angles);
    }
    else if (StrEqual(infectedName, "charger", false))
    {
        spawnedEntity = L4D2_SpawnSpecial(L4D2_ZOMBIE_CLASS_CHARGER, position, angles);
    }
    else if (StrEqual(infectedName, "tank", false))
    {
        spawnedEntity = L4D2_SpawnTank(position, angles);
    }
    else if (StrEqual(infectedName, "witch", false))
    {
        spawnedEntity = L4D2_SpawnWitch(position, angles);
    }
    else
    {
        PrintToServer(
            "[ChaosForge] Unsupported infected type: %s.",
            infectedName
        );

        return -1;
    }

    if (spawnedEntity <= 0)
    {
        PrintToServer(
            "[ChaosForge] Native %s spawn failed.",
            infectedName
        );

        return -1;
    }

    PrintToServer(
        "[ChaosForge] Spawned %s entity %d for Survivor %N at X %.2f, Y %.2f, Z %.2f (trace hit entity %d).",
        infectedName,
        spawnedEntity,
        resolvedClient,
        position[0],
        position[1],
        position[2],
        hitEntity
    );

    return spawnedEntity;
}

bool ChaosSpawn_SpecialInfected(
    const char[] infectedName,
    int preferredClient = 0
)
{
    return ChaosSpawn_SpecialInfectedEntity(
        infectedName,
        preferredClient
    ) > 0;
}

bool ChaosSpawn_Hunter(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected("hunter", preferredClient);
}

bool ChaosSpawn_Smoker(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected("smoker", preferredClient);
}

bool ChaosSpawn_Boomer(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected("boomer", preferredClient);
}

bool ChaosSpawn_Charger(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected("charger", preferredClient);
}

bool ChaosSpawn_Jockey(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected("jockey", preferredClient);
}

bool ChaosSpawn_Spitter(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected("spitter", preferredClient);
}

bool ChaosSpawn_Tank(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected("tank", preferredClient);
}

bool ChaosSpawn_Witch(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected("witch", preferredClient);
}

bool ChaosSpawn_Common(
    int count,
    int preferredClient = 0
)
{
    int spawningClient =
        ChaosSpawn_GetAvailableSurvivor(preferredClient);

    if (spawningClient == 0)
    {
        return false;
    }

    if (count < 1)
    {
        count = 1;
    }

    int originalFlags = GetCommandFlags("z_spawn_old");

    if (originalFlags == INVALID_FCVAR_FLAGS)
    {
        return false;
    }

    SetCommandFlags(
        "z_spawn_old",
        originalFlags & ~FCVAR_CHEAT
    );

    for (int i = 0; i < count; i++)
    {
        FakeClientCommand(
            spawningClient,
            "z_spawn_old infected auto"
        );
    }

    SetCommandFlags(
        "z_spawn_old",
        originalFlags
    );

    return true;
}
