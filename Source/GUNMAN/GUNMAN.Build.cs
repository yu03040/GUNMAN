// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GUNMAN : ModuleRules
{
    public GUNMAN(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "EnhancedInput", "AnimGraphRuntime", "NavigationSystem", "UMG" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    }
}
