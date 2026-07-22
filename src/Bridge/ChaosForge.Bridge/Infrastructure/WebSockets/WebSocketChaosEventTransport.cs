using ChaosForge.Core.Transport;
using ChaosForge.Shared.Contracts;

namespace ChaosForge.Bridge.Infrastructure.WebSockets;

public sealed class WebSocketChaosEventTransport
    : IChaosEventTransport
{
    private readonly ChaosForgeWebSocketConnection _connection;

    public WebSocketChaosEventTransport(
        ChaosForgeWebSocketConnection connection)
    {
        _connection = connection;
    }

    public Task<ChaosExecutionResult> SendAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default)
    {
        return _connection.SendEventAsync(
            chaosEvent,
            cancellationToken);
    }
}