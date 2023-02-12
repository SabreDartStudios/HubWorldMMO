// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class OWSHubWorldMMOEditorTarget : TargetRules
{
	public OWSHubWorldMMOEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.AddRange( new string[] { "OWSHubWorldMMO" } );

        /*
        //Only works with a source build engine
        if (!bUseIris)
        {
            BuildEnvironment = TargetBuildEnvironment.Unique;
            bUseIris = true;
        }
        */
    }
}
