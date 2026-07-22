using ChaosForge.Core.Transport;
using ChaosForge.Shared.Contracts;
using Microsoft.Extensions.Logging;

namespace ChaosForge.Core.Handlers;

public sealed class SpawnCommonInfectedHandler
    : IChaosEventHandler
{
    private readonly IChaosEventTransport _transport;
    private readonly ILogger<SpawnCommonInfectedHandler> _logger;

    public SpawnCommonInfectedHandler(
        IChaosEventTransport transport,
        ILogger<SpawnCommonInfectedHandler> logger)
    {
        _transport = transport;
        _logger = logger;
    }

    public ChaosEventType EventType =>
        ChaosEventType.SpawnCommonInfected;

    public async Task<ChaosExecutionResult> ExecuteAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(chaosEvent);

        try
        {
            _logger.LogInformation(
                "Dispatching common infected event {EventId}. Viewer: {ViewerName}, Count: {Count}",
                chaosEvent.Id,
                chaosEvent.ViewerName,
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
                "Failed to dispatch common infected event {EventId}.",
                chaosEvent.Id);

            return ChaosExecutionResult.Failed(
                chaosEvent,
                exception.Message);
        }
    }
}