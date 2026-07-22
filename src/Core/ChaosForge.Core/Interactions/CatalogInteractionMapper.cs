using ChaosForge.Shared.Contracts;
using ChaosForge.Shared.Interactions;

namespace ChaosForge.Core.Interactions;

public sealed class CatalogInteractionMapper
    : IInteractionMapper
{
    private readonly IInteractionMappingCatalog _catalog;

    public CatalogInteractionMapper(
        IInteractionMappingCatalog catalog)
    {
        _catalog = catalog;
    }

    public Task<InteractionMappingResult> MapAsync(
        ViewerInteraction interaction,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(interaction);

        InteractionMappingDefinition? mapping =
            _catalog.FindMapping(interaction);

        if (mapping is null)
        {
            return Task.FromResult(
                InteractionMappingResult.Failed(
                    $"No enabled mapping exists for source " +
                    $"'{interaction.Source}', interaction type " +
                    $"'{interaction.Type}', and trigger " +
                    $"'{interaction.TriggerName}'."));
        }

        var parameters =
            new Dictionary<string, string>(
                mapping.Parameters,
                StringComparer.OrdinalIgnoreCase);

        foreach (KeyValuePair<string, string> parameter
                 in interaction.Parameters)
        {
            parameters[parameter.Key] = parameter.Value;
        }

        SpecialInfectedType? infected = null;

        if (parameters.TryGetValue(
                "infected",
                out string? infectedValue)
            && !string.IsNullOrWhiteSpace(infectedValue))
        {
            if (!Enum.TryParse(
                    infectedValue,
                    ignoreCase: true,
                    out SpecialInfectedType parsedInfected)
                || parsedInfected == SpecialInfectedType.Unknown)
            {
                return Task.FromResult(
                    InteractionMappingResult.Failed(
                        $"Unsupported infected type " +
                        $"'{infectedValue}'."));
            }

            infected = parsedInfected;
        }

        if (mapping.EventType ==
                ChaosEventType.SpawnSpecialInfected
            && infected is null)
        {
            return Task.FromResult(
                InteractionMappingResult.Failed(
                    $"Mapping '{mapping.Id}' requires an " +
                    "'infected' parameter."));
        }

        int quantity = Math.Max(
            1,
            interaction.Quantity);

        int multiplier = Math.Max(
            1,
            mapping.CountMultiplier);

        int count = Math.Clamp(
            quantity * multiplier,
            1,
            100);

        parameters.TryGetValue(
            "giftName",
            out string? giftName);

        if (string.IsNullOrWhiteSpace(giftName)
            && interaction.Type == InteractionType.Gift)
        {
            giftName = interaction.TriggerName;
        }

        var chaosEvent = new ChaosEvent
        {
            Id = interaction.Id,
            Type = mapping.EventType,
            ViewerName = interaction.ViewerName,
            GiftName = giftName,
            Count = count,
            Infected = infected,
            CreatedAt = interaction.ReceivedAt
        };

        return Task.FromResult(
            InteractionMappingResult.Mapped(
                chaosEvent));
    }
}