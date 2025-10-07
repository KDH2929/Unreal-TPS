// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PixelMario : ModuleRules
{
	public PixelMario(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "RenderCore", "UMG", "NavigationSystem" });
        PrivateDependencyModuleNames.AddRange(new string[] { "ProceduralMeshComponent" });
    }
}
