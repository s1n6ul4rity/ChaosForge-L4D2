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

bool ChaosSpawn_Hunter(int preferredClient = 0)
{
    int spawningClient = ChaosSpawn_GetAvailableSurvivor(preferredClient);

    if (spawningClient == 0)
    {
        PrintToServer("[ChaosForge] No living Survivor is available.");
        return false;
    }

    int originalFlags = GetCommandFlags("z_spawn_old");

    if (originalFlags == INVALID_FCVAR_FLAGS)
    {
        PrintToServer("[ChaosForge] Could not find z_spawn_old.");
        return false;
    }

    SetCommandFlags(
        "z_spawn_old",
        originalFlags & ~FCVAR_CHEAT
    );

    FakeClientCommand(
        spawningClient,
        "z_spawn_old hunter auto"
    );

    SetCommandFlags(
        "z_spawn_old",
        originalFlags
    );

    PrintToServer(
        "[ChaosForge] Hunter spawn requested using Survivor %N.",
        spawningClient
    );

    return true;
}
