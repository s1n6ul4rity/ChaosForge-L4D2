#pragma semicolon 1
#pragma newdecls required

#include <sourcemod>
#include <sdktools>
#include <left4dhooks>
#include <websocket>
#include <websocket/yyjson.inc>

#define CHAOSFORGE_VERSION "0.3.1"

#include "chaosforge/spawn_manager.inc"
#include "chaosforge/dispatcher.inc"
#include "chaosforge/game_state.inc"
#include "chaosforge/spawn_queue.inc"
#include "chaosforge/protocol.inc"
#include "chaosforge/websocket_client.inc"
#include "chaosforge/commands.inc"

public Plugin myinfo =
{
    name = "ChaosForge",
    author = "s1n6ul4rity",
    description = "TikTok LIVE chaos integration for Left 4 Dead 2",
    version = CHAOSFORGE_VERSION,
    url = "https://github.com/s1n6ul4rity/ChaosForge-L4D2"
};

public void OnPluginStart()
{
    ChaosCommands_Register();
    ChaosGameState_Initialize();
    ChaosWebSocket_Connect();

    HookEvent(
        "player_death",
        ChaosForgeEvent_PlayerDeath,
        EventHookMode_Post
    );

    PrintToServer(
        "[ChaosForge] Plugin loaded successfully. Version %s",
        CHAOSFORGE_VERSION
    );
}

public void OnPluginEnd()
{
    ChaosWebSocket_Disconnect();
}

public void ChaosForgeEvent_PlayerDeath(
    Event event,
    const char[] name,
    bool dontBroadcast
)
{
    int userId = event.GetInt("userid");
    int client = GetClientOfUserId(userId);

    if (client <= 0
        || client > MaxClients
        || !IsClientInGame(client))
    {
        return;
    }

    // Only infected deaths should wake the special-infected queue.
    if (GetClientTeam(client) != 3)
    {
        return;
    }

    PrintToServer(
        "[ChaosForge] Infected client %d died. Waking the spawn queue.",
        client
    );

    ChaosSpawnQueue_Wake();
}