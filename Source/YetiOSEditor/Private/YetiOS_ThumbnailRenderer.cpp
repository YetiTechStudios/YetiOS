// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#include "YetiOS_ThumbnailRenderer.h"
#include "Core/YetiOS_DirectoryBase.h"
#include "CanvasItem.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Core/YetiOS_FileBase.h"
#include "Core/YetiOS_Core.h"
#include "Misc/YetiOS_ProgramsRepository.h"

void UYetiOS_ThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(Object);
	if (Blueprint)
	{
		DrawTextureItem(Canvas, Width, Height, GetTextureFromGeneratedClass(Blueprint->GeneratedClass));

		const UYetiOS_DirectoryBase* BaseDirectory = GetDefaultObjectOfClass<UYetiOS_DirectoryBase>(Blueprint->GeneratedClass);
		if (BaseDirectory)
		{
			DrawTextItem(Canvas, FVector2D(10.f, 10.f), "Name", BaseDirectory->GetDirectoryName().ToString());
			DrawTextItem(Canvas, FVector2D(10.f, 30.f), "Total Child Dirs", FString::FromInt(BaseDirectory->ChildDirectoryClasses.Num()));
			DrawTextItem(Canvas, FVector2D(10.f, 50.f), "Total Files", FString::FromInt(BaseDirectory->FileClasses.Num()));
			
			return;
		}

		const UYetiOS_BaseProgram* BaseProgram = GetDefaultObjectOfClass<UYetiOS_BaseProgram>(Blueprint->GeneratedClass);
		if (BaseProgram)
		{
			DrawTextItem(Canvas, FVector2D(10.f, 10.f), "Name", BaseProgram->GetProgramName().ToString());
			DrawTextItem(Canvas, FVector2D(10.f, 30.f), "Identifier", BaseProgram->GetProgramIdentifierName().ToString());
			DrawTextItem(Canvas, FVector2D(10.f, 50.f), "Add to Desktop", BaseProgram->CanAddToDesktop() ? "Yes" : "No");

			return;
		}

		const UYetiOS_FileBase* BaseFile = GetDefaultObjectOfClass<UYetiOS_FileBase>(Blueprint->GeneratedClass);
		if (BaseFile)
		{
			DrawTextItem(Canvas, FVector2D(10.f, 10.f), "Name", BaseFile->GetFilename(true).ToString());
			DrawTextItem(Canvas, FVector2D(10.f, 30.f), "Size", FString::FromInt(FMath::TruncToInt(BaseFile->GetFileSize())));
			DrawTextItem(Canvas, FVector2D(10.f, 50.f), "", BaseFile->IsHidden() ? "Hidden" : "Not hidden");
			DrawTextItem(Canvas, FVector2D(10.f, 70.f), "", BaseFile->IsDeletable() ? "Deletable" : "Not deletable");
			DrawTextItem(Canvas, FVector2D(10.f, 90.f), "", BaseFile->IsMovable() ? "Movable" : "Not movable");

			return;
		}

		const UYetiOS_Core* OS = GetDefaultObjectOfClass<UYetiOS_Core>(Blueprint->GeneratedClass);
		if (OS)
		{
			DrawTextItem(Canvas, FVector2D(10.f, 10.f), "Name", OS->GetOsName().ToString());
			DrawTextItem(Canvas, FVector2D(10.f, 30.f), "Version", UYetiOS_Core::GetVersionString(OS->GetOsVersion()));
			DrawTextItem(Canvas, FVector2D(10.f, 50.f), "Supported Devices", FString::FromInt(OS->CompatibleDevices.Num()));

			return;
		}
	}

	Super::Draw(Object, X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily);
}

bool UYetiOS_ThumbnailRenderer::CanVisualizeAsset(UObject* Object)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(Object);

	if (Blueprint)
	{
		if (GetDefaultObjectOfClass<UYetiOS_DirectoryBase>(Blueprint->GeneratedClass))
		{
			return true;
		}
		else if (GetDefaultObjectOfClass<UYetiOS_BaseProgram>(Blueprint->GeneratedClass))
		{
			return true;
		}
		else if (GetDefaultObjectOfClass<UYetiOS_FileBase>(Blueprint->GeneratedClass))
		{
			return true;
		}
		else if (GetDefaultObjectOfClass<UYetiOS_Core>(Blueprint->GeneratedClass))
		{
			return true;
		}
	}

	return Super::CanVisualizeAsset(Object);
}

void UYetiOS_ThumbnailRenderer::DrawTextItem(FCanvas*& Canvas, const FVector2D& AtLocation, const FString& InSuffix, const FString& InString)
{
	static const FLinearColor TextColor = FLinearColor(0.75, 0.75, 0.75);
	static const UFont* TextFont = GEngine->GetLargeFont();
	const FString TextMessage = InSuffix.IsEmpty() ? InString : FString::Printf(TEXT("%s: %s"), *InSuffix, *InString);
	FCanvasTextItem TextItem(AtLocation, FText::FromString(TextMessage), TextFont, TextColor);
	TextItem.Draw(Canvas);
}

void UYetiOS_ThumbnailRenderer::DrawTextureItem(FCanvas*& Canvas, const int32& Width, const int32& Height, UTexture2D* Texture2D)
{
	if (Texture2D)
	{
		const FVector2D HalfSize = FVector2D(Width / 2.f, Height / 2.f);
		FCanvasTileItem CanvasTile(HalfSize, Texture2D->Resource, HalfSize, FLinearColor::White);
		CanvasTile.BlendMode = SE_BLEND_Opaque;
		CanvasTile.Draw(Canvas);
		if (Texture2D && Texture2D->IsCurrentlyVirtualTextured())
		{
			const TCHAR* VTChars = TEXT("VT");
			int32 VTWidth = 0;
			int32 VTHeight = 0;
			StringSize(GEngine->GetLargeFont(), VTWidth, VTHeight, VTChars);
			float PaddingX = Width / 128.0f;
			float PaddingY = Height / 128.0f;
			float ScaleX = Width / 64.0f; //Text is 1/64'th of the size of the thumbnails
			float ScaleY = Height / 64.0f;
			// VT overlay
			FCanvasTextItem TextItem(FVector2D(Width - PaddingX - VTWidth * ScaleX, Height - PaddingY - VTHeight * ScaleY), FText::FromString(VTChars), GEngine->GetLargeFont(), FLinearColor::White);
			TextItem.EnableShadow(FLinearColor::Black);
			TextItem.Scale = FVector2D(ScaleX, ScaleY);
			TextItem.Draw(Canvas);
		}
	}
}

UTexture2D* UYetiOS_ThumbnailRenderer::GetTextureFromGeneratedClass(UClass* Class) const
{
	if (Class)
	{
		const UYetiOS_DirectoryBase* DirObject = GetDefaultObjectOfClass<UYetiOS_DirectoryBase>(Class);
		if (DirObject)
		{
			return DirObject->GetDirectoryIcon();
		}

		const UYetiOS_BaseProgram* BaseProgram = GetDefaultObjectOfClass<UYetiOS_BaseProgram>(Class);
		if (BaseProgram)
		{
			return Cast<UTexture2D>(BaseProgram->GetProgramIcon());
		}

		const UYetiOS_FileBase* BaseFile = GetDefaultObjectOfClass<UYetiOS_FileBase>(Class);
		if (BaseFile)
		{
			return Cast<UTexture2D>(BaseFile->GetFileIcon());
		}

		const UYetiOS_Core* OS = GetDefaultObjectOfClass<UYetiOS_Core>(Class);
		if (OS)
		{
			return Cast<UTexture2D>(OS->GetOsIcon());
		}
	}

	return nullptr;
}
