namespace ChaosForge.Bridge.Configuration;

public sealed class BridgeSettings
{
    public const string SectionName = "Bridge";

    public string Host { get; init; } = "127.0.0.1";

    public int Port { get; init; } = 6721;

    public string ApiKey { get; init; } = string.Empty;
}