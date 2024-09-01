using UnrealBuildTool;

public class ProjectEast : ModuleRules
{
	public ProjectEast(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange
		(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"UMG",
				"EnhancedInput"
			});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AnimGraphRuntime",
			"Slate", 
			"SlateCore" 
		});
	}
}