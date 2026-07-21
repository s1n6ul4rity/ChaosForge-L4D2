#if defined _chaosforge_dispatcher_included
    #endinput
#endif
#define _chaosforge_dispatcher_included

bool ChaosDispatcher_Dispatch(
    WebSocket socket,
    const char[] eventType,
    const char[] infected,
    int count,
    bool hasInfected
)
{
    if (!StrEqual(eventType, "SpawnSpecialInfected", false))
    {
        socket.WriteString("ERROR UNKNOWN_EVENT_TYPE");
        return false;
    }

    if (!hasInfected)
    {
        socket.WriteString("ERROR MISSING_INFECTED");
        return false;
    }

    if (count < 1)
    {
        count = 1;
    }

    for (int i = 0; i < count; i++)
    {
        if (!ChaosDispatcher_SpawnInfected(infected))
        {
            socket.WriteString("ERROR SPAWN_SPECIAL_INFECTED");
            return false;
        }
    }

    socket.WriteString("OK SPAWN_SPECIAL_INFECTED");
    return true;
}

bool ChaosDispatcher_SpawnInfected(
    const char[] infected
)
{
    if (StrEqual(infected, "Hunter", false))
    {
        return ChaosSpawn_Hunter();
    }

    if (StrEqual(infected, "Smoker", false))
    {
        return ChaosSpawn_Smoker();
    }

    if (StrEqual(infected, "Boomer", false))
    {
        return ChaosSpawn_Boomer();
    }

    if (StrEqual(infected, "Charger", false))
    {
        return ChaosSpawn_Charger();
    }

    if (StrEqual(infected, "Jockey", false))
    {
        return ChaosSpawn_Jockey();
    }

    if (StrEqual(infected, "Spitter", false))
    {
        return ChaosSpawn_Spitter();
    }

    if (StrEqual(infected, "Tank", false))
    {
        return ChaosSpawn_Tank();
    }

    if (StrEqual(infected, "Witch", false))
    {
        return ChaosSpawn_Witch();
    }

    PrintToServer(
        "[ChaosForge] Unsupported infected type: %s",
        infected
    );

    return false;
}