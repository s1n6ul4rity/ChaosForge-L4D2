using ChaosForge.Core.Handlers;
using ChaosForge.Core.Transport;
using ChaosForge.Shared.Contracts;
using Microsoft.Extensions.Logging;

namespace ChaosForge.Core.Handlers;

public sealed class SpawnSpecialInfectedHandler : IChaosEventHandler
{
    private readonly IChaosEventTransport _transport;
    private readonly ILogger<SpawnSpecialInfectedHandler> _logger;

    public SpawnSpecialInfectedHandler(
        IChaosEventTransport transport,
        ILogger<SpawnSpecialInfectedHandler> logger)
    {
        _transport = transport;
        _logger = logger;
    }

    public ChaosEventType EventType =>
        ChaosEventType.SpawnSpecialInfected;

    public async Task<ChaosExecutionResult> ExecuteAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(chaosEvent);

        try
        {
            _logger.LogInformation(
                "Dispatching special infected event {EventId}. Viewer: {ViewerName}, Infected: {InfectedType}, Count: {Count}",
                chaosEvent.Id,
                chaosEvent.ViewerName,
                chaosEvent.Infected,
                chaosEvent.Count);

            return await _transport.SendAsync(
        chaosEvent,
        cancellationToken);
        }
        catch (OperationCanceledException)
        {
            throw;
        }
        catch (Exception exception)
        {
            _logger.LogError(
                exception,
                "Failed to dispatch special infected event {EventId}.",
                chaosEvent.Id);

            return ChaosExecutionResult.Failed(
                chaosEvent,
                exception.Message);
        }
    }
}