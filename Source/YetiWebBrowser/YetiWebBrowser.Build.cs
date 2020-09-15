// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class YetiWebBrowser : ModuleRules
{
	public YetiWebBrowser(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"ApplicationCore",
				"RHI",
				"InputCore",
				"Serialization",
				"HTTP",
				"UMG",
				"Projects"
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore"
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Win64
		||  Target.Platform == UnrealTargetPlatform.Win32
		||  Target.Platform == UnrealTargetPlatform.Mac
		||  Target.Platform == UnrealTargetPlatform.Linux)
		{
			//PrivateDependencyModuleNames.Add("CEF3Utils");
			//AddEngineThirdPartyPrivateStaticDependencies(Target,
			//	"CEF3"
			//	);
			PrivateDependencyModuleNames.Add("CEF");
			PublicIncludePaths.Add(CefSourceCodeBasePath);
			if (Target.Type != TargetType.Server)
			{
				if (Target.Platform == UnrealTargetPlatform.Mac)
				{
					// Add contents of UnrealCefSubProcess.app directory as runtime dependencies
					foreach (string FilePath in Directory.EnumerateFiles(Target.RelativeEnginePath + "/Binaries/Mac/UnrealCEFSubProcess.app", "*", SearchOption.AllDirectories))
					{
						RuntimeDependencies.Add(FilePath);
					}
				}
				else if (Target.Platform == UnrealTargetPlatform.Linux)
				{
					RuntimeDependencies.Add("$(EngineDir)/Binaries/" + Target.Platform.ToString() + "/UnrealCEFSubProcess");
				}
				else
				{
					RuntimeDependencies.Add("$(EngineDir)/Binaries/" + Target.Platform.ToString() + "/UnrealCEFSubProcess.exe");
				}
			}
		}

		if (Target.Platform == UnrealTargetPlatform.PS4 &&
			Target.bCompileAgainstEngine)
		{
			PrivateDependencyModuleNames.Add("Engine");
		}

		if(Target.Platform == UnrealTargetPlatform.Lumin)
		{
			PrecompileForTargets = ModuleRules.PrecompileTargetsType.None;
		}
	}

	private string CefSourceCodeBasePath
	{
		get { return Path.Combine(ModuleDirectory, "../ThirdParty", "CEF", "cef_source"); }
	}

	private string CefIncludeFiles
	{
		get { return Path.Combine(CefSourceCodeBasePath, "include"); }
	}
}
