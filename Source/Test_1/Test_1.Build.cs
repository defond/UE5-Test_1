// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Test_1 : ModuleRules
{
	public Test_1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", 
			"GameplayAbilities", "GameplayTags", "GameplayTasks" }); 
    }
}
