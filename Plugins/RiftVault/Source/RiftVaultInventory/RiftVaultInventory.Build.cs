using UnrealBuildTool;

public class RiftVaultInventory : ModuleRules
{
    public RiftVaultInventory(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "NetCore",
            "RiftVaultCore",
            "RiftVaultSave"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "CoreUObject",
            "Engine",
            "Json",
            "Niagara",
            "UMG",
            "Slate",
            "SlateCore"
        });

        AddVersionBasedDependencies(Target);
        SetupIrisSupport(Target);
    }

    public void AddVersionBasedDependencies(ReadOnlyTargetRules Target)
    {
        if (Target.Version is { MajorVersion: 5, MinorVersion: < 5 })
        {
            PublicDependencyModuleNames.Add("StructUtils");
        }
    }
}
