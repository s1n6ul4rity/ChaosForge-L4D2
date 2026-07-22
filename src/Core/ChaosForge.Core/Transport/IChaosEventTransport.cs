using ChaosForge.Shared.Contracts;

namespace ChaosForge.Core.Transport;

public interface IChaosEventTransport
{
    Task<ChaosExecutionResult> SendAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default);
}