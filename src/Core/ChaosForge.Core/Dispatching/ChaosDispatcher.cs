using ChaosForge.Core.Handlers;
using ChaosForge.Shared.Contracts;

namespace ChaosForge.Core.Dispatching;

public sealed class ChaosDispatcher : IChaosDispatcher
{
    private readonly IReadOnlyDictionary<
        ChaosEventType,
        IChaosEventHandler> _handlers;

    public ChaosDispatcher(
        IEnumerable<IChaosEventHandler> handlers)
    {
        ArgumentNullException.ThrowIfNull(handlers);

        _handlers = handlers.ToDictionary(
            handler => handler.EventType,
            handler => handler);
    }

    public Task<ChaosExecutionResult> DispatchAsync(
        ChaosEvent chaosEvent,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(chaosEvent);

        if (!_handlers.TryGetValue(
                chaosEvent.Type,
                out var handler))
        {
            return Task.FromResult(
                ChaosExecutionResult.Failed(
                    chaosEvent,
                    $"No handler is registered for event type " +
                    $"'{chaosEvent.Type}'."));
        }

        return handler.ExecuteAsync(
            chaosEvent,
            cancellationToken);
    }
}