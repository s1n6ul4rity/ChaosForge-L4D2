using ChaosForge.Shared.Contracts;

namespace ChaosForge.Core.Handlers;

public interface IChaosEventHandler
{
    ChaosEventType EventType { get; }

    Task<ChaosExecutionResult> ExecuteAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default);
}