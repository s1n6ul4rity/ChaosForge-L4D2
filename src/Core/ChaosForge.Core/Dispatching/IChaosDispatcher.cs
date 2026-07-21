using ChaosForge.Shared.Contracts;

namespace ChaosForge.Core.Dispatching;

public interface IChaosDispatcher
{
    Task<ChaosExecutionResult> DispatchAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default);
}