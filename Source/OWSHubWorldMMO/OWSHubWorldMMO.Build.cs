// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class OWSHubWorldMMO : ModuleRules
{
	public OWSHubWorldMMO(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject",
			"NetCore",
			"Engine",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
			"AIModule"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
            "InputCore",
            "EnhancedInput",
            "Json",
			"JsonUtilities",
			"HTTP",
            "OWSPlugin"
        });

		PublicIncludePathModuleNames.AddRange(new string[] { 
			"OWSPlugin" 
		});

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        //Add Gameplay Debugger Support
        SetupGameplayDebuggerSupport(Target);
        //Add Iris Support
        SetupIrisSupport(Target);

    }
}
