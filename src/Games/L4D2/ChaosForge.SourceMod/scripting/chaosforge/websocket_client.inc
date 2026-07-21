#if defined _chaosforge_websocket_client_included
    #endinput
#endif
#define _chaosforge_websocket_client_included

#define CHAOSFORGE_BRIDGE_URL "ws://127.0.0.1:6721/chaosforge"

WebSocket g_ChaosWebSocket = null;

void ChaosWebSocket_Connect()
{
    if (g_ChaosWebSocket != null)
    {
        delete g_ChaosWebSocket;
        g_ChaosWebSocket = null;
    }

    g_ChaosWebSocket = new WebSocket(
        CHAOSFORGE_BRIDGE_URL,
        Websocket_STRING
    );

    g_ChaosWebSocket.SetOpenCallback(
        ChaosWebSocket_OnOpen
    );

    g_ChaosWebSocket.SetMessageCallback(
        ChaosWebSocket_OnMessage
    );

    g_ChaosWebSocket.SetCloseCallback(
        ChaosWebSocket_OnClose
    );

    g_ChaosWebSocket.SetErrorCallback(
        ChaosWebSocket_OnError
    );

    g_ChaosWebSocket.AutoReconnect = true;
    g_ChaosWebSocket.MinReconnectWait = 1000;
    g_ChaosWebSocket.MaxReconnectWait = 10000;
    g_ChaosWebSocket.HandshakeTimeout = 10;
    g_ChaosWebSocket.PingInterval = 20;

    PrintToServer(
        "[ChaosForge] Connecting to Bridge at %s...",
        CHAOSFORGE_BRIDGE_URL
    );

    g_ChaosWebSocket.Connect();
}

void ChaosWebSocket_Disconnect()
{
    if (g_ChaosWebSocket == null)
    {
        return;
    }

    g_ChaosWebSocket.AutoReconnect = false;

    if (g_ChaosWebSocket.Connected)
    {
        g_ChaosWebSocket.Disconnect();
    }

    delete g_ChaosWebSocket;
    g_ChaosWebSocket = null;
}

bool ChaosWebSocket_IsConnected()
{
    return g_ChaosWebSocket != null
        && g_ChaosWebSocket.Connected;
}

public void ChaosWebSocket_OnOpen(WebSocket socket)
{
    PrintToServer("[ChaosForge] Connected to Bridge.");

    socket.WriteString("HELLO CHAOSFORGE_L4D2");
}

public void ChaosWebSocket_OnMessage(
    WebSocket socket,
    const char[] message,
    int wireSize
)
{
    ChaosProtocol_HandleMessage(
        socket,
        message
    );
}

public void ChaosWebSocket_OnClose(
    WebSocket socket,
    int code,
    const char[] reason
)
{
    PrintToServer(
        "[ChaosForge] Bridge connection closed. Code: %d. Reason: %s",
        code,
        reason
    );
}

public void ChaosWebSocket_OnError(
    WebSocket socket,
    const char[] errorMessage
)
{
    LogError(
        "[ChaosForge] WebSocket error: %s",
        errorMessage
    );
}
