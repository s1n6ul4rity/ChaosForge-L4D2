using ChaosForge.Shared.Contracts;
using ChaosForge.Shared.Interactions;

namespace ChaosForge.Core.Interactions;

public interface IInteractionPipeline
{
    Task<ChaosExecutionResult> ExecuteAsync(
        ViewerInteraction interaction,
        CancellationToken cancellationToken = default);
}