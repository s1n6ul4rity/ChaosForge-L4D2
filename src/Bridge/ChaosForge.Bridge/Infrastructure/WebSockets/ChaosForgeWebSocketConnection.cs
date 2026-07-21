using ChaosForge.Shared.Contracts;
using System.Net.WebSockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace ChaosForge.Bridge.Infrastructure.WebSockets;

public sealed class ChaosForgeWebSocketConnection : IAsyncDisposable
{
    private static readonly JsonSerializerOptions SerializerOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        Converters =
        {
            new JsonStringEnumConverter()
        }
    };

    private readonly SemaphoreSlim _sendLock = new(1, 1);
    private WebSocket? _socket;

    public bool IsConnected =>
        _socket is { State: WebSocketState.Open };

    public void Attach(WebSocket socket)
    {
        ArgumentNullException.ThrowIfNull(socket);
        _socket = socket;
    }

    public void Detach(WebSocket socket)
    {
        if (ReferenceEquals(_socket, socket))
        {
            _socket = null;
        }
    }

    public async Task SendAsync(
    string message,
    CancellationToken cancellationToken = default)
    {
        if (_socket is not { State: WebSocketState.Open } socket)
        {
            throw new InvalidOperationException(
                "The ChaosForge SourceMod plugin is not connected.");
        }

        byte[] payload = Encoding.UTF8.GetBytes(message);

        await _sendLock.WaitAsync(cancellationToken);

        try
        {
            await socket.SendAsync(
                payload,
                WebSocketMessageType.Text,
                endOfMessage: true,
                cancellationToken);
        }
        finally
        {
            _sendLock.Release();
        }
    }

    public async Task SendAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(chaosEvent);

        if (_socket is not { State: WebSocketState.Open } socket)
        {
            throw new InvalidOperationException(
                "The ChaosForge SourceMod plugin is not connected.");
        }

        string json = JsonSerializer.Serialize(
            chaosEvent,
            SerializerOptions);

        byte[] payload = Encoding.UTF8.GetBytes(json);

        await _sendLock.WaitAsync(cancellationToken);

        try
        {
            await socket.SendAsync(
                payload,
                WebSocketMessageType.Text,
                endOfMessage: true,
                cancellationToken);
        }
        finally
        {
            _sendLock.Release();
        }
    }

    public async ValueTask DisposeAsync()
    {
        if (_socket is { State: WebSocketState.Open } socket)
        {
            await socket.CloseAsync(
                WebSocketCloseStatus.NormalClosure,
                "ChaosForge Bridge shutting down.",
                CancellationToken.None);
        }

        _socket?.Dispose();
        _sendLock.Dispose();
    }
}