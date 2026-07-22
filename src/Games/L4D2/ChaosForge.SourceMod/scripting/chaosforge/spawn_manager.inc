#if defined _chaosforge_spawn_manager_included
    #endinput
#endif
#define _chaosforge_spawn_manager_included

#define CHAOSFORGE_TEAM_SURVIVOR 2

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

bool ChaosSpawn_SpecialInfected(
    const char[] infectedName,
    int preferredClient = 0
)
{
    int spawningClient =
        ChaosSpawn_GetAvailableSurvivor(preferredClient);

    if (spawningClient == 0)
    {
        PrintToServer(
            "[ChaosForge] No living Survivor is available."
        );

        return false;
    }

    int originalFlags = GetCommandFlags("z_spawn_old");

    if (originalFlags == INVALID_FCVAR_FLAGS)
    {
        PrintToServer(
            "[ChaosForge] Could not find z_spawn_old."
        );

        return false;
    }

    SetCommandFlags(
        "z_spawn_old",
        originalFlags & ~FCVAR_CHEAT
    );

    FakeClientCommand(
        spawningClient,
        "z_spawn_old %s auto",
        infectedName
    );

    SetCommandFlags(
        "z_spawn_old",
        originalFlags
    );

    PrintToServer(
        "[ChaosForge] %s spawn requested using Survivor %N.",
        infectedName,
        spawningClient
    );

    return true;
}

bool ChaosSpawn_Hunter(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected(
        "hunter",
        preferredClient
    );
}

bool ChaosSpawn_Smoker(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected(
        "smoker",
        preferredClient
    );
}

bool ChaosSpawn_Boomer(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected(
        "boomer",
        preferredClient
    );
}

bool ChaosSpawn_Charger(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected(
        "charger",
        preferredClient
    );
}

bool ChaosSpawn_Jockey(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected(
        "jockey",
        preferredClient
    );
}

bool ChaosSpawn_Spitter(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected(
        "spitter",
        preferredClient
    );
}

bool ChaosSpawn_Tank(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected(
        "tank",
        preferredClient
    );
}

bool ChaosSpawn_Witch(int preferredClient = 0)
{
    return ChaosSpawn_SpecialInfected(
        "witch",
        preferredClient
    );
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
        PrintToServer(
            "[ChaosForge] No living Survivor is available."
        );

        return false;
    }

    if (count < 1)
    {
        count = 1;
    }

    int originalFlags = GetCommandFlags("z_spawn_old");

    if (originalFlags == INVALID_FCVAR_FLAGS)
    {
        PrintToServer(
            "[ChaosForge] Could not find z_spawn_old."
        );

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

    PrintToServer(
        "[ChaosForge] Requested %d common infected using Survivor %N.",
        count,
        spawningClient
    );

    return true;
}