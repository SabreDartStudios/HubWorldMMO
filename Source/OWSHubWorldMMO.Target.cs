// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class OWSHubWorldMMOTarget : TargetRules
{
	public OWSHubWorldMMOTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.AddRange( new string[] { "OWSHubWorldMMO" } );

        /*
        if (!bUseIris)
        {
            BuildEnvironment = TargetBuildEnvironment.Unique;
            bUseIris = true;
        }
        */
    }
}
