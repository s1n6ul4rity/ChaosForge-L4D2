namespace ChaosForge.Bridge.Endpoints;

public static class HealthEndpoints
{
    public static IEndpointRouteBuilder MapHealthEndpoints(
        this IEndpointRouteBuilder endpoints)
    {
        endpoints.MapGet("/", () =>
        {
            return Results.Text("ChaosForge Bridge Online");
        });

        endpoints.MapGet("/ping", () =>
        {
            return Results.Ok(new
            {
                name = "ChaosForge",
                status = "Online",
                version = "0.1.0"
            });
        });

        return endpoints;
    }
}