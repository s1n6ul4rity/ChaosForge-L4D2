namespace ChaosForge.Shared.Protocol;

public static class ChaosProtocol
{
    public const int CurrentVersion = 1;

    public static class MessageTypes
    {
        public const string ChaosEvent = "ChaosEvent";

        public const string EventResult = "EventResult";
    }

    public static class Platforms
    {
        public const string Development = "Development";

        public const string TikTok = "TikTok";

        public const string Twitch = "Twitch";

        public const string YouTube = "YouTube";
    }
}