// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GUNMANEditorTarget : TargetRules
{
	public GUNMANEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		CppStandard = CppStandardVersion.Cpp20;
        ExtraModuleNames.Add("GUNMAN");
	}
}
