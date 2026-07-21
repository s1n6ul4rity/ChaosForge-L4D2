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
    }
    else
    {
        PrintToServer("%s", message);
    }
}
