using ChaosForge.Shared.Interactions;

namespace ChaosForge.Core.Interactions;

public interface IInteractionMappingCatalog
{
    InteractionMappingDefinition? FindMapping(
        ViewerInteraction interaction);
}