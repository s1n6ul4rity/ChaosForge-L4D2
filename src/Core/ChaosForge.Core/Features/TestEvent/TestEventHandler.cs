using ChaosForge.Core.Handlers;
using ChaosForge.Shared.Contracts;
using Microsoft.Extensions.Logging;

namespace ChaosForge.Core.Features.TestEvent;

public sealed class TestEventHandler : IChaosEventHandler
{
    private readonly ILogger<TestEventHandler> _logger;

    public TestEventHandler(ILogger<TestEventHandler> logger)
    {
        _logger = logger;
    }

    public ChaosEventType EventType => ChaosEventType.TestEvent;

    public Task<ChaosExecutionResult> ExecuteAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(chaosEvent);

        _logger.LogInformation(
            "Executing test event {EventId} from viewer {ViewerName}. Count: {Count}",
            chaosEvent.Id,
            chaosEvent.ViewerName,
            chaosEvent.Count);

        var result = ChaosExecutionResult.Succeeded(
            chaosEvent,
            chaosEvent.Count,
            $"Test event successfully handled for {chaosEvent.ViewerName}.");

        return Task.FromResult(result);
    }
}