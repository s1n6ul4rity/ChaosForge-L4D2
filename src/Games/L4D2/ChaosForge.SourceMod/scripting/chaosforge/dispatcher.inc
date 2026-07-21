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

    if (!StrEqual(infected, "Hunter", false))
    {
        socket.WriteString("ERROR UNSUPPORTED_INFECTED");
        return false;
    }

    if (count < 1)
    {
        count = 1;
    }

    for (int i = 0; i < count; i++)
    {
        if (!ChaosSpawn_Hunter())
        {
            socket.WriteString("ERROR SPAWN_SPECIAL_INFECTED");
            return false;
        }
    }

    socket.WriteString("OK SPAWN_SPECIAL_INFECTED");
    return true;
}