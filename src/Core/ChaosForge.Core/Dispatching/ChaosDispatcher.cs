using ChaosForge.Core.Handlers;
using ChaosForge.Shared.Contracts;

namespace ChaosForge.Core.Dispatching;

public sealed class ChaosDispatcher : IChaosDispatcher
{
    private readonly IReadOnlyCollection<IChaosEventHandler> _handlers;

    public ChaosDispatcher(
        IEnumerable<IChaosEventHandler> handlers)
    {
        ArgumentNullException.ThrowIfNull(handlers);

        _handlers = handlers.ToArray();
    }

    public async Task<ChaosExecutionResult> DispatchAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(chaosEvent);

        IChaosEventHandler? handler =
            _handlers.FirstOrDefault(
                candidate =>
                    candidate.EventType == chaosEvent.Type);

        if (handler is null)
        {
            return ChaosExecutionResult.Failed(
                chaosEvent,
                $"No handler is registered for event type '{chaosEvent.Type}'.");
        }

        return await handler.ExecuteAsync(
            chaosEvent,
            cancellationToken);
    }
}