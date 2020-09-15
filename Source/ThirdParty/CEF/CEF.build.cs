// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System.Collections.Generic;

public class CEF : ModuleRules
{
	public CEF(ReadOnlyTargetRules Target) : base(Target)
	{
		/** Mark the current version of the library */
		string CEFVersion = "85.2.11.g0202816";
		string CEFPlatform = "";

		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			CEFPlatform = "windows64";
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			// CEFPlatform = "macosx64";
			throw new BuildException("Mac not supported yet.");
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			// CEFVersion = "3.2623.1395.g3034273";
			// CEFPlatform = "linux64";
			throw new BuildException("Linux not supported yet.");
		}

		if (CEFPlatform.Length > 0)
		{
			string PlatformPath = Path.Combine(CEFThirdPartyPath, "cef_binary_" + CEFVersion + "_" + CEFPlatform);

			PublicSystemIncludePaths.Add(PlatformPath);

			string LibraryPath = Path.Combine(PlatformPath, "Release");
			string RuntimePath = BinaryPath;

			if (Target.Platform == UnrealTargetPlatform.Win64)
			{
				// There are different versions of the C++ wrapper lib depending on the version of VS we're using
				string VSVersionFolderName = "VS2019"; // + Target.WindowsPlatform.GetVisualStudioCompilerVersionName();
				string WrapperLibraryPath = Path.Combine(PlatformPath, VSVersionFolderName, "libcef_dll");
				WrapperLibraryPath += "/Release";

				AddLibrary(Path.Combine(LibraryPath, "libcef.lib"));
				AddLibrary(Path.Combine(WrapperLibraryPath, "libcef_dll_wrapper.lib"));

				List<string> Dlls = new List<string>();

				Dlls.Add("chrome_elf.dll");
				Dlls.Add("d3dcompiler_47.dll");
				Dlls.Add("libcef.dll");
				Dlls.Add("libEGL.dll");
				Dlls.Add("libGLESv2.dll");

				PublicDelayLoadDLLs.AddRange(Dlls);

				// Add the runtime dlls to the build receipt
				foreach (string Dll in Dlls)
				{
					AddRuntimeDependency(Path.Combine(BinaryPath, Dll));
				}
				// We also need the icu translations table required by CEF
				AddRuntimeDependency(Path.Combine(BinaryPath, "icudtl.dat"));

				// Add the V8 binary data files as well
				AddRuntimeDependency(Path.Combine(BinaryPath, "v8_context_snapshot.bin"));
				AddRuntimeDependency(Path.Combine(BinaryPath, "snapshot_blob.bin"));

				// And the entire Resources folder. Enumerate the entire directory instead of mentioning each file manually here.
				foreach (string FileName in Directory.EnumerateFiles(Path.Combine(RuntimePath, "Resources"), "*", SearchOption.AllDirectories))
				{
					AddRuntimeDependency(Path.Combine(ModuleDirectory, "../../Binaries/ThirdParty/", FileName));
				}
			}
			// TODO: Ensure these are filled out correctly when adding other platforms
			else if (Target.Platform == UnrealTargetPlatform.Mac)
			{
				string WrapperPath = LibraryPath + "/libcef_dll_wrapper.a";
				string FrameworkPath = Target.UEThirdPartyBinariesDirectory + "CEF3/Mac/Chromium Embedded Framework.framework";

				PublicAdditionalLibraries.Add(WrapperPath);
				PublicFrameworks.Add(FrameworkPath);

				if (Directory.Exists(LibraryPath + "/locale"))
				{
					var LocaleFolders = Directory.GetFileSystemEntries(LibraryPath + "/locale", "*.lproj");
					foreach (var FolderName in LocaleFolders)
					{
						AdditionalBundleResources.Add(new BundleResource(FolderName, bShouldLog: true));
					}
				}

				// Add contents of framework directory as runtime dependencies
				foreach (string FilePath in Directory.EnumerateFiles(FrameworkPath, "*", SearchOption.AllDirectories))
				{
					RuntimeDependencies.Add(FilePath);
				}
			}
			else if (Target.Platform == UnrealTargetPlatform.Linux)
			{
				// link against runtime library since this produces correct RPATH
				string RuntimeLibCEFPath = Path.Combine(RuntimePath, "libcef.so");
				PublicAdditionalLibraries.Add(RuntimeLibCEFPath);

				string Configuration = "build_release";
				string WrapperLibraryPath = Path.Combine(PlatformPath, Configuration, "libcef_dll");

				PublicAdditionalLibraries.Add(Path.Combine(WrapperLibraryPath, "libcef_dll_wrapper.a"));

				PrivateRuntimeLibraryPaths.Add("$(EngineDir)/Binaries/ThirdParty/CEF3/" + Target.Platform.ToString());

				RuntimeDependencies.Add("$(EngineDir)/Binaries/ThirdParty/CEF3/" + Target.Platform.ToString() + "/libcef.so");
				RuntimeDependencies.Add("$(EngineDir)/Binaries/ThirdParty/CEF3/" + Target.Platform.ToString() + "/icudtl.dat");
				RuntimeDependencies.Add("$(EngineDir)/Binaries/ThirdParty/CEF3/" + Target.Platform.ToString() + "/natives_blob.bin");
				RuntimeDependencies.Add("$(EngineDir)/Binaries/ThirdParty/CEF3/" + Target.Platform.ToString() + "/snapshot_blob.bin");

				// And the entire Resources folder. Enunerate the entire directory instead of mentioning each file manually here.
				foreach (string FileName in Directory.EnumerateFiles(Path.Combine(RuntimePath, "Resources"), "*", SearchOption.AllDirectories))
				{
					string DependencyName = FileName.Substring(Target.UEThirdPartyBinariesDirectory.Length).Replace('\\', '/');
					RuntimeDependencies.Add("$(EngineDir)/Binaries/ThirdParty/" + DependencyName);
				}
			}
		}
	}

	private void AddRuntimeDependency(string InFullPath)
	{
		string FullPath = Path.GetFullPath(InFullPath);
		if (File.Exists(FullPath) == false)
		{
			throw new BuildException(string.Format("{0} does not exist in {1}!", Path.GetFileName(FullPath), Path.GetDirectoryName(FullPath)));
		}

		RuntimeDependencies.Add(FullPath);
		System.Console.WriteLine(string.Format("Dependency added {0}", FullPath));
	}

	private void AddLibrary(string InFullPath)
	{
		if (File.Exists(InFullPath) == false)
		{
			throw new BuildException(string.Format("{0} does not exist in {1}!", Path.GetFileName(InFullPath), Path.GetFullPath(InFullPath)));
		}

		PublicAdditionalLibraries.Add(InFullPath);
		System.Console.WriteLine(string.Format("Library added {0}", InFullPath.Replace(ModuleDirectory, "")));
	}

	private string BinaryPath
	{
		get
		{
			return Path.Combine(ModuleDirectory, "../../../Binaries/ThirdParty/CEF/", Target.Platform.ToString());
		}
	}

	private string CEFThirdPartyPath
	{
		get
		{
			return ModuleDirectory;
		}
	}
}
