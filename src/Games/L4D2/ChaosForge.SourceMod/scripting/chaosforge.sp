#pragma semicolon 1
#pragma newdecls required

#include <sourcemod>
#include <websocket>

#define CHAOSFORGE_VERSION "0.3.0"

#include "chaosforge/spawn_manager.inc"
#include "chaosforge/commands.inc"
#include "chaosforge/protocol.inc"
#include "chaosforge/websocket_client.inc"

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
    ChaosWebSocket_Connect();

    PrintToServer(
        "[ChaosForge] Plugin loaded successfully. Version %s",
        CHAOSFORGE_VERSION
    );
}

public void OnPluginEnd()
{
    ChaosWebSocket_Disconnect();
}
