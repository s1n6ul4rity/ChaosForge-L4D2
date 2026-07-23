#if defined _chaosforge_commands_included
    #endinput
#endif
#define _chaosforge_commands_included

void ChaosCommands_Register()
{
    RegConsoleCmd(
        "sm_chaosforge_ping",
        ChaosCommand_Ping,
        "Tests whether the ChaosForge SourceMod plugin is responding."
    );

    RegConsoleCmd(
        "sm_chaosforge_spawnhunter",
        ChaosCommand_SpawnHunter,
        "Spawns a Hunter near a living Survivor."
    );

    RegConsoleCmd(
        "sm_chaosforge_crosshair",
        ChaosCommand_Crosshair,
        "Prints the world position currently targeted by a living Survivor."
    );

    RegConsoleCmd(
        "sm_chaosforge_spawnhuntercrosshair",
        ChaosCommand_SpawnHunterCrosshair,
        "Spawns one Hunter at the world position targeted by a living Survivor."
    );

    RegConsoleCmd(
        "sm_chaosforge_wsstatus",
        ChaosCommand_WebSocketStatus,
        "Displays the ChaosForge WebSocket connection status."
    );
}

public Action ChaosCommand_Ping(int client, int args)
{
    ChaosCommand_Reply(
        client,
        "[ChaosForge] PONG - SourceMod integration is online."
    );

    return Plugin_Handled;
}

public Action ChaosCommand_Crosshair(int client, int args)
{
    float position[3];
    int resolvedClient;
    int hitEntity;

    if (!ChaosTarget_GetCrosshairPosition(
        client,
        position,
        resolvedClient,
        hitEntity
    ))
    {
        ChaosCommand_Reply(
            client,
            "[ChaosForge] No valid crosshair target was found."
        );

        return Plugin_Handled;
    }

    char message[256];
    Format(
        message,
        sizeof(message),
        "[ChaosForge] Crosshair hit: X %.2f | Y %.2f | Z %.2f | Entity %d | Survivor %N",
        position[0],
        position[1],
        position[2],
        hitEntity,
        resolvedClient
    );

    ChaosCommand_Reply(client, message);

    PrintToServer(
        "[ChaosForge] Crosshair target for Survivor %N: X %.2f, Y %.2f, Z %.2f, entity %d.",
        resolvedClient,
        position[0],
        position[1],
        position[2],
        hitEntity
    );

    return Plugin_Handled;
}

public Action ChaosCommand_SpawnHunterCrosshair(int client, int args)
{
    int spawnedEntity;
    int resolvedClient;
    float position[3];

    bool succeeded = ChaosSpawn_HunterAtCrosshair(
        client,
        spawnedEntity,
        resolvedClient,
        position
    );

    if (!succeeded)
    {
        ChaosCommand_Reply(
            client,
            "[ChaosForge] Hunter crosshair spawn failed. Check the server console for details."
        );

        return Plugin_Handled;
    }

    char message[256];
    Format(
        message,
        sizeof(message),
        "[ChaosForge] Hunter spawned at X %.2f | Y %.2f | Z %.2f | Entity %d | Survivor %N",
        position[0],
        position[1],
        position[2],
        spawnedEntity,
        resolvedClient
    );

    ChaosCommand_Reply(client, message);

    return Plugin_Handled;
}

public Action ChaosCommand_SpawnHunter(int client, int args)
{
    bool succeeded = ChaosSpawn_Hunter(client);

    ChaosCommand_Reply(
        client,
        succeeded
            ? "[ChaosForge] Hunter spawn requested."
            : "[ChaosForge] Hunter spawn failed."
    );

    return Plugin_Handled;
}

public Action ChaosCommand_WebSocketStatus(int client, int args)
{
    ChaosCommand_Reply(
        client,
        ChaosWebSocket_IsConnected()
            ? "[ChaosForge] WebSocket is connected."
            : "[ChaosForge] WebSocket is disconnected."
    );

    return Plugin_Handled;
}

void ChaosCommand_Reply(int client, const char[] message)
{
    if (client > 0 && IsClientInGame(client))
    {
        ReplyToCommand(client, "%s", message);
        return;
    }

    PrintToServer("%s", message);
}
