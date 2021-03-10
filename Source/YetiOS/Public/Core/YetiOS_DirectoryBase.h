// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_Types.h"
#include "Templates/SubclassOf.h"
#include "YetiOS_DirectoryBase.generated.h"

/*************************************************************************
* File Information:
YetiOS_DirectoryBase.h

* Description:
Base class for all directories.
*************************************************************************/
UCLASS(hidedropdown, Blueprintable, DisplayName = "Base Directory")
class YETIOS_API UYetiOS_DirectoryBase : public UObject
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	friend class UYetiOS_ThumbnailRenderer;
#endif

protected:

	/** Name of this directory. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Directory Base")
	FText DirectoryName;
	
	/** An icon for this directory. Like a folder icon. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Directory Base")
	UTexture2D* DirectoryIcon;

	/** Type of this directory. Any type that is not Other is considered system directory. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Directory Base")
	EDirectoryType DirectoryType;

	/** True if user can create new folder in this directory. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Directory Base")
	uint8 bCanCreateNewFolder : 1;

	/** True if user can create new file in this directory. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Directory Base")
	uint8 bCanCreateNewFile : 1;

	/** True if this folder is hidden initially. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Directory Base")
	uint8 bIsHidden : 1;

	/** List of files in this directory. Only valid if Can Create New File is checked. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Directory Base", meta = (EditCondition = "bCanCreateNewFile"))
	TSet<TSubclassOf<class UYetiOS_FileBase>> FileClasses;

	/** List of child directories for this directory. Only valid if Can Create New Folder is checked. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Directory Base", meta = (EditCondition = "bCanCreateNewFolder"))
	TArray<TSubclassOf<UYetiOS_DirectoryBase>> ChildDirectoryClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Directory Base")
	TSet<TSubclassOf<class UYetiOS_BaseProgram>> ProgramClasses;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Directory Base")
	FYetiOS_Lock LockedUsers;

	/** Parent directory */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	UYetiOS_DirectoryBase* ParentDirectory;

	/** Set of files in this directory. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TSet<class UYetiOS_FileBase*> Files;

	/** List of programs in this directory */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TSet<class UYetiOS_BaseProgram*> Programs;

	/** List of child directories. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<UYetiOS_DirectoryBase*> ChildDirectories;

	/** Operating System that owns this directory. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_Core* OwningOS;

public:

	UYetiOS_DirectoryBase();

	static bool AddProgramToDirectory(UYetiOS_DirectoryBase* InDirectory, class UYetiOS_BaseProgram* ProgramToAdd);
	static void RemoveProgramFromDirectory(UYetiOS_DirectoryBase* InDirectory, class UYetiOS_BaseProgram* ProgramToRemove);

	/**
	* public UYetiOS_DirectoryBase::GetDirectoryName const
	* Returns the DirectoryName.
	* @return [const FText] Name of this Directory.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")
	const FText GetDirectoryName() const { return DirectoryName; }

	/**
	* public UYetiOS_DirectoryBase::GetDirectoryIcon const
	* Returns the icon for this directory.
	* @return [class UTexture2D*] Icon of this directory.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")
	class UTexture2D* GetDirectoryIcon() const { return DirectoryIcon; }

	/**
	* public UYetiOS_DirectoryBase::GetChildDirectory const
	* Gets the child directory based on given class.
	* @param InDirectoryClass [TSubclassOf<UYetiDirectoryBase>] Class to check for.
	* @return [UYetiDirectoryBase*] Directory reference from ChildDirectories.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")
	UYetiOS_DirectoryBase* GetChildDirectory(TSubclassOf<UYetiOS_DirectoryBase> InDirectoryClass) const;

	/**
	* public UYetiOS_DirectoryBase::GetChildDirectoryByType const
	* Returns a child directory of given type. Can be null so check for validity.
	* @param InType [EDirectoryType] Type of directory to return.
	* @return [UYetiOS_DirectoryBase*] Found directory of type. Can be null so check for validity.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")	
	UYetiOS_DirectoryBase* GetChildDirectoryByType(EDirectoryType InType) const;

	/**
	* public UYetiOS_DirectoryBase::GetChildDirectoryByName const
	* Gets the child directory by name.
	* @param InDirectoryName [const FName&] Name of the directory.
	* @param bRecursive [const bool] If true look for the directory name in all child directories and dig deeper into them too.
	* @return [UYetiOS_DirectoryBase*] Directory reference.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")	
	UYetiOS_DirectoryBase* GetChildDirectoryByName(const FName& InDirectoryName, const bool bRecursive = true) const;

	/**
	* public UYetiOS_DirectoryBase::GetDirectoryFromPath const
	* Get directory from given path. So if you pass /home/desktop/mydirectory this will return mydirectory from the path if it exists.
	* @param InPath [const FString&] Path to check.
	* @return [UYetiOS_DirectoryBase*] Reference to the directory that was found or nullptr.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")	
	UYetiOS_DirectoryBase* GetDirectoryFromPath(const FString& InPath) const;

	/**
	* public UYetiOS_DirectoryBase::CanCreateNewFolder const
	* Checks if this directory can create child directories.
	* @return [const bool] Returns true if this can create child directory.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")
	const bool CanCreateNewFolder() const { return bCanCreateNewFolder; }

	/**
	* public UYetiOS_DirectoryBase::GetAllChildDirectories const
	* Gets all the child directories for this directory.
	* @See GetChildDirectory()
	* @return [TArray<UYetiDirectoryBase*>] An array of all child directories.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")
	TArray<class UYetiOS_DirectoryBase*> GetAllChildDirectories() const { return ChildDirectories; }

	/**
	* public UYetiOS_Directory::GetDirectoryFiles const
	* Returns files in this directory.
	* @param WithExtension [const FString] Extension to filter by. Set this to * or . or leave empty to get all files.
	* @return [TSet<class UYetiOS_FileBase*>] List of file objects.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")
	TSet<class UYetiOS_FileBase*> GetDirectoryFiles(const FString WithExtension = "*") const;

	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")
	TArray<class UYetiOS_BaseProgram*> GetPrograms() const { return Programs.Array(); }

	/**
	* public UYetiOS_Directory::IsHidden const
	* Checks if this directory is hidden or not.
	* @return [const bool] Returns true if this directory is hidden or not.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")
	const bool IsHidden() const { return bIsHidden; }

	/**
	* public UYetiOS_Directory::GetParentDirectory const
	* Get Parent Directory for this directory.
	* @return [UYetiOS_DirectoryBase*] Parent Directory.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")
	UYetiOS_DirectoryBase* GetParentDirectory() const { return ParentDirectory; }

	/**
	* virtual public UYetiOS_DirectoryRoot::ToggleLock
	* Given user will be locked for this file and won't be able to open it.
	* @param bLock [const bool] True to lock. False to unlock
	* @param InUser [const FYetiOsUser&] The user to be locked out of this file.
	**/
	virtual void ToggleLock(const bool bLock, const FYetiOsUser& InUser);

	/**
	* public UYetiOS_DirectoryBase::CreateChildDirectory
	* Creates a new directory in this directory.
	* @param InOwningOS [const UYetiOS_Core*] OS that owns this directory.
	* @param InDirectoryClass [TSubclassOf<UYetiOS_DirectoryBase>] New directory class. Its better to create a template Blueprint class and assign it here.
	* @param InDirectoryName [const FText&] Name of the directory.
	* @param bCreateHidden [const bool] If true, set to hidden after creating.
	* @param bForceCreate [const bool] If true, forcefully create new folder even if Can Create New Folder is false.
	* @return [UYetiOS_DirectoryBase*] Reference to the newly created directory.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Directory Base", meta = (AdvancedDisplay = "2"))
	UYetiOS_DirectoryBase* CreateChildDirectory(class UYetiOS_Core* InOwningOS, TSubclassOf<UYetiOS_DirectoryBase> InDirectoryClass, FYetiOsError& OutErrorMessage, const FText& InDirectoryName, const bool bCreateHidden = false, const bool bForceCreate = false);

	/**
	* public UYetiOS_DirectoryBase::CreateChildDirectories
	* Creates child directories of given directory classes.
	* @param InOwningOS [const UYetiOS_Core*] OS that owns this directory.
	* @param InDirectoryClasses [const TArray<TSubclassOf<class UYetiOS_DirectoryBase>>&] Directory classes.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @param bForceCreate [const bool] Forcefully creates child directory even if this folder cannot create new directory. Ignores Can Create New Folder.
	* @return [TArray<UYetiOS_DirectoryBase*>] Reference to the list of newly created child directories. This does not include grand child directories.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Directory Base")
	TArray<UYetiOS_DirectoryBase*> CreateChildDirectories(class UYetiOS_Core* InOwningOS, const TArray<TSubclassOf<UYetiOS_DirectoryBase>>& InDirectoryClasses, FYetiOsError& OutErrorMessage, const bool bForceCreate /* = false */);
	
	/**
	* public UYetiOS_DirectoryBase::CreateNativeChildDirectories
	* Create default child directories specified in this class. @See ChildDirectoryClasses variable.
	* @See Internal_CreateChildDirectories() method.
	* @param InOwningOS [const UYetiOS_Core*] OS that owns this directory.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @param bForceCreate [const bool] Forcefully creates child directory even if this folder cannot create new directory. Ignores bCanCreateNewFolder.
	* @param bCreateGrandChildDirectories [const bool] Continue to create child directories inside child directories.
	* @return [TArray<UYetiOS_DirectoryBase*>] Reference to the list of newly created child directories. This does not include grand child directories.
	**/
	TArray<UYetiOS_DirectoryBase*> CreateNativeChildDirectories(class UYetiOS_Core* InOwningOS, FYetiOsError& OutErrorMessage, const bool bForceCreate = false, const bool bCreateGrandChildDirectories = true);

	/**
	* public UYetiOS_DirectoryBase::CreateNewFileByClass
	* Creates a new file in this directory.
	* @param InNewFileClass [TSubclassOf<class UYetiOS_FileBase>] File class to create.
	* @param OutFile [class UYetiOS_FileBase*&] Reference to the newly created file (if valid).
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @param bRequirePermission [const bool] Forcefully creates a file even if bCanCreateNewFile is false.
	* @return [bool] Returns true if the file was created successfully.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Directory Base")	
	bool CreateNewFileByClass(TSubclassOf<class UYetiOS_FileBase> InNewFileClass, class UYetiOS_FileBase*& OutFile, FYetiOsError& OutErrorMessage, const bool bRequirePermission = false);

	/**
	* public UYetiOS_DirectoryBase::GetAllParentDirectories const
	* Returns an array of all parent directories.
	* @param bIncludeRootFolder [const bool] True to include root directory in result.
	* @return [TArray<UYetiOS_DirectoryBase*>] Array of parents.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base")	
	TArray<UYetiOS_DirectoryBase*> GetAllParentDirectories(const bool bIncludeRootFolder = false) const;

	/**
	* public UYetiOS_DirectoryBase::IsLockedForUser const
	* Checks if the given user is locked for this file.
	* @param InUser [const FYetiOsUser&] User to check.
	* @return [bool] True if locked. False if not.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	bool IsLockedForUser(const FYetiOsUser& InUser) const { return LockedUsers.IsUserAllowed(InUser) == false; }

	/**
	* public UYetiOS_DirectoryBase::GetLockedUsers const
	* Returns an array of locked users.
	* @return [TArray<FYetiOsUser>] Array containing information of users that are locked from this directory.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	inline TArray<FYetiOsUser> GetLockedUsers() const { return LockedUsers.GetLockedUsers().Array(); }

	/**
	* public UYetiOS_DirectoryBase::EnsureOS
	* Makes sure that OwningOS is set.
	* @param InOS [const class UYetiOS_Core *] OS to assign if OwningOS is nullptr.
	**/
	void EnsureOS(const class UYetiOS_Core* InOS);

private:

	/**
	* private UYetiOS_DirectoryBase::Internal_CreateChildDirectories
	* Creates child directories from given classes.
	* @param InOwningOS [const UYetiOS_Core*] OS that owns this directory.
	* @param InDirectoryClasses [const TArray<TSubclassOf<UYetiOS_DirectoryBase>>&]  Directory classes.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @param bForceCreate [const bool] Forcefully creates child directory even if this folder cannot create new directory. Ignores bCanCreateNewFolder.
	* @param bCreateGrandChildDirectories [const bool] Continue to create child directories inside child directories.
	* @param CheckDirectoryName [const FText&] Override default directory name with this name. 
	* @return [TArray<UYetiOS_DirectoryBase*>] Reference to the list of newly created child directories. This does not include grand child directories.
	**/
	TArray<UYetiOS_DirectoryBase*> Internal_CreateChildDirectories(class UYetiOS_Core* InOwningOS, 
		const TArray<TSubclassOf<UYetiOS_DirectoryBase>>& InDirectoryClasses, 
		FYetiOsError& OutErrorMessage, 
		const bool bForceCreate = false, 
		const bool bCreateGrandChildDirectories = true, 
		const FText& CheckDirectoryName = FText::GetEmpty());

public:

	/**
	* public UYetiOS_DirectoryBase::DestroyDirectory
	* Destroys child directories and self.
	**/
	void DestroyDirectory();

	/**
	* public UYetiOS_DirectoryBase::GetDirectoryPath const
	* Get full path of this directory. Eg: Dir1/Dir2/Dir3
	* @return [FText] Full path.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Directory Base", meta = (Keywords = "full"))	
	FText GetDirectoryPath() const;

	/**
	* public UYetiOS_DirectoryBase::GetFullPath const
	* Returns a string representation of full path.
	* @param bDoNoIncludeHome [const bool] Enable to skip home directory.
	* @return [FString] Returns string of full path of this directory.
	**/
	inline FString GetFullPath(const bool bDoNoIncludeHome = false) const;

	/**
	* public UYetiOS_DirectoryBase::HasChildDirectory const
	* Checks if this directory has a child directory with given name.
	* @param InDirectoryName [const FText &] Name to check for.
	* @param OutFoundDirectory [UYetiOS_DirectoryBase*&] Contains a valid reference to the directory if it exists.
	* @param bIgnoreCase [const bool] Case-insensitive if true.
	* @return [const bool] True if a directory with given name exists.
	**/
	inline const bool HasChildDirectory(const FText& InDirectoryName, UYetiOS_DirectoryBase*& OutFoundDirectory, const bool bIgnoreCase = true) const;

	FORCEINLINE const bool IsSystemDirectory() const { return DirectoryType != EDirectoryType::Other; }
	FORCEINLINE const bool CanCreateNewFile() const { return bCanCreateNewFile; }
	FORCEINLINE const TArray<TSubclassOf<UYetiOS_DirectoryBase>> GetChildDirectories() const { return ChildDirectoryClasses; }
	FORCEINLINE const EDirectoryType GetDirectoryType() const  { return DirectoryType; }
	FORCEINLINE class UYetiOS_Core* GetOwningOS() const { return OwningOS; }
};
