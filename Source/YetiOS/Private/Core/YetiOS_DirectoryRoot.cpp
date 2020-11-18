// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_DirectoryRoot.h"

UYetiOS_DirectoryRoot::UYetiOS_DirectoryRoot()
{
	bCanCreateNewFolder = false;
	bCanCreateNewFile = false;
	bIsHidden = true;
	bIsSystemDirectory = true;
	DirectoryType = EDirectoryType::Root;
}

void UYetiOS_DirectoryRoot::AddSystemDirectory(UYetiOS_DirectoryBase* InDirectoryToAdd)
{
	if (InDirectoryToAdd && ensureMsgf(InDirectoryToAdd->GetDirectoryType() != EDirectoryType::Other, TEXT("%s cannot be added as system directory."), *InDirectoryToAdd->GetDirectoryName().ToString()))
	{
		SystemDirectories.Add(InDirectoryToAdd);
	}	
}
