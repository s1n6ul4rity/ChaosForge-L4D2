using ChaosForge.Shared.Interactions;

namespace ChaosForge.Core.Interactions;

public sealed class InteractionMappingCatalog
    : IInteractionMappingCatalog
{
    private readonly IReadOnlyCollection<InteractionMappingDefinition>
        _mappings;

    public InteractionMappingCatalog(
        IEnumerable<InteractionMappingDefinition> mappings)
    {
        ArgumentNullException.ThrowIfNull(mappings);

        _mappings = mappings
            .Where(mapping => mapping.Enabled)
            .ToArray();
    }

    public InteractionMappingDefinition? FindMapping(
        ViewerInteraction interaction)
    {
        ArgumentNullException.ThrowIfNull(interaction);

        return _mappings.FirstOrDefault(
            mapping =>
                string.Equals(
                    mapping.Source,
                    interaction.Source,
                    StringComparison.OrdinalIgnoreCase)
                && mapping.InteractionType == interaction.Type
                && string.Equals(
                    mapping.TriggerName,
                    interaction.TriggerName,
                    StringComparison.OrdinalIgnoreCase));
    }
}