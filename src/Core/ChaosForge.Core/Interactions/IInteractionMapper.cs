using ChaosForge.Shared.Interactions;

namespace ChaosForge.Core.Interactions;

public interface IInteractionMapper
{
    Task<InteractionMappingResult> MapAsync(
        ViewerInteraction interaction,
        CancellationToken cancellationToken = default);
}