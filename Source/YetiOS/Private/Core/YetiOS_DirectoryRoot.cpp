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
