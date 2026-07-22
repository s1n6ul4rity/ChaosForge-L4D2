using ChaosForge.Shared.Contracts;
using ChaosForge.Shared.Protocol;
using System.Collections.Concurrent;
using System.Net.WebSockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace ChaosForge.Bridge.Infrastructure.WebSockets;

public sealed class ChaosForgeWebSocketConnection : IAsyncDisposable
{
    private static readonly TimeSpan DefaultResponseTimeout =
        TimeSpan.FromSeconds(10);

    private static readonly JsonSerializerOptions SerializerOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition =
            JsonIgnoreCondition.WhenWritingNull,
        PropertyNameCaseInsensitive = true,
        Converters =
        {
            new JsonStringEnumConverter()
        }
    };

    private readonly SemaphoreSlim _sendLock = new(1, 1);

    private readonly ConcurrentDictionary<
        Guid,
        TaskCompletionSource<ChaosExecutionResult>>
        _pendingEvents = new();

    private WebSocket? _socket;

    public bool IsConnected =>
        _socket is { State: WebSocketState.Open };

    public void Attach(
        WebSocket socket)
    {
        ArgumentNullException.ThrowIfNull(socket);

        _socket = socket;
    }

    public void Detach(
        WebSocket socket)
    {
        if (!ReferenceEquals(
            _socket,
            socket))
        {
            return;
        }

        _socket = null;

        FailAllPendingEvents(
            "The SourceMod plugin disconnected.");
    }

    public async Task SendAsync(
        string message,
        CancellationToken cancellationToken = default)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(
            message);

        await SendTextAsync(
            message,
            cancellationToken);
    }

    public async Task<ChaosExecutionResult> SendEventAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(
            chaosEvent);

        if (!IsConnected)
        {
            return ChaosExecutionResult.Failed(
                chaosEvent,
                "The ChaosForge SourceMod plugin is not connected.");
        }

        var completionSource =
            new TaskCompletionSource<ChaosExecutionResult>(
                TaskCreationOptions.RunContinuationsAsynchronously);

        if (!_pendingEvents.TryAdd(
                chaosEvent.Id,
                completionSource))
        {
            return ChaosExecutionResult.Failed(
                chaosEvent,
                $"Event '{chaosEvent.Id}' is already awaiting a response.");
        }

        try
        {
            ChaosEventMessage eventMessage =
                ChaosEventMessage.FromChaosEvent(
                    chaosEvent);

            string json = JsonSerializer.Serialize(
                eventMessage,
                SerializerOptions);

            Console.WriteLine(
                $"[Bridge] Sending protocol v{eventMessage.ProtocolVersion} {eventMessage.MessageType}: {json}");

            await SendTextAsync(
                json,
                cancellationToken);

            using var timeoutSource =
                new CancellationTokenSource(
                    DefaultResponseTimeout);

            using var linkedSource =
                CancellationTokenSource.CreateLinkedTokenSource(
                    cancellationToken,
                    timeoutSource.Token);

            try
            {
                return await completionSource.Task.WaitAsync(
                    linkedSource.Token);
            }
            catch (OperationCanceledException)
                when (timeoutSource.IsCancellationRequested
                      && !cancellationToken.IsCancellationRequested)
            {
                return ChaosExecutionResult.Failed(
                    chaosEvent,
                    $"SourceMod did not acknowledge the event within {DefaultResponseTimeout.TotalSeconds:0} seconds.");
            }
        }
        catch (OperationCanceledException)
        {
            throw;
        }
        catch (Exception exception)
        {
            return ChaosExecutionResult.Failed(
                chaosEvent,
                exception.Message);
        }
        finally
        {
            _pendingEvents.TryRemove(
                chaosEvent.Id,
                out _);
        }
    }

    public bool TryHandleAcknowledgement(
        string message)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(
            message);

        ChaosEventAcknowledgement? acknowledgement;

        try
        {
            acknowledgement =
                JsonSerializer.Deserialize<ChaosEventAcknowledgement>(
                    message,
                    SerializerOptions);
        }
        catch (JsonException)
        {
            return false;
        }

        if (acknowledgement is null
            || acknowledgement.EventId == Guid.Empty
            || string.IsNullOrWhiteSpace(
                acknowledgement.MessageType))
        {
            return false;
        }

        if (!string.Equals(
                acknowledgement.MessageType,
                ChaosProtocol.MessageTypes.EventResult,
                StringComparison.Ordinal))
        {
            return false;
        }

        if (!acknowledgement.IsSupported)
        {
            Console.WriteLine(
                $"[Bridge] Rejected unsupported acknowledgement protocol version {acknowledgement.ProtocolVersion}. Expected version {ChaosProtocol.CurrentVersion}.");

            CompletePendingEventWithFailure(
                acknowledgement.EventId,
                $"Unsupported SourceMod protocol version {acknowledgement.ProtocolVersion}.");

            return true;
        }

        if (!_pendingEvents.TryRemove(
                acknowledgement.EventId,
                out var completionSource))
        {
            Console.WriteLine(
                $"[Bridge] Received acknowledgement for unknown event {acknowledgement.EventId}.");

            return true;
        }

        var result = new ChaosExecutionResult
        {
            EventId = acknowledgement.EventId,
            Success = acknowledgement.Success,
            RequestedCount =
                acknowledgement.RequestedCount,
            ExecutedCount =
                acknowledgement.ExecutedCount,
            Message = acknowledgement.Message
        };

        completionSource.TrySetResult(
            result);

        Console.WriteLine(
            $"[Bridge] Event {acknowledgement.EventId} acknowledged through protocol v{acknowledgement.ProtocolVersion}. Success: {acknowledgement.Success}, Executed: {acknowledgement.ExecutedCount}/{acknowledgement.RequestedCount}");

        return true;
    }

    private async Task SendTextAsync(
        string message,
        CancellationToken cancellationToken)
    {
        if (_socket is not
            {
                State: WebSocketState.Open
            } socket)
        {
            throw new InvalidOperationException(
                "The ChaosForge SourceMod plugin is not connected.");
        }

        byte[] payload =
            Encoding.UTF8.GetBytes(
                message);

        await _sendLock.WaitAsync(
            cancellationToken);

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

    private void CompletePendingEventWithFailure(
        Guid eventId,
        string message)
    {
        if (!_pendingEvents.TryRemove(
                eventId,
                out var completionSource))
        {
            return;
        }

        completionSource.TrySetResult(
            new ChaosExecutionResult
            {
                EventId = eventId,
                Success = false,
                RequestedCount = 0,
                ExecutedCount = 0,
                Message = message
            });
    }

    private void FailAllPendingEvents(
        string message)
    {
        foreach (var pendingEvent in _pendingEvents)
        {
            if (!_pendingEvents.TryRemove(
                    pendingEvent.Key,
                    out var completionSource))
            {
                continue;
            }

            completionSource.TrySetResult(
                new ChaosExecutionResult
                {
                    EventId = pendingEvent.Key,
                    Success = false,
                    RequestedCount = 0,
                    ExecutedCount = 0,
                    Message = message
                });
        }
    }

    public async ValueTask DisposeAsync()
    {
        FailAllPendingEvents(
            "The ChaosForge Bridge is shutting down.");

        if (_socket is
            {
                State: WebSocketState.Open
            } socket)
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