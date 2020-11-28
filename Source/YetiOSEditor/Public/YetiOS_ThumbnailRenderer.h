// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThumbnailRendering/BlueprintThumbnailRenderer.h"
#include "YetiOS_ThumbnailRenderer.generated.h"

UCLASS(MinimalAPI)
class UYetiOS_ThumbnailRenderer : public UBlueprintThumbnailRenderer
{
	GENERATED_BODY()
	
public:

	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily) override;
	virtual bool CanVisualizeAsset(UObject* Object) override;

protected:

	template<class T>
	static FORCEINLINE const T* GetDefaultObjectOfClass(const UClass* Class)
	{
		if (Class->IsChildOf(T::StaticClass()))
		{
			return Class->GetDefaultObject<T>();
		}

		return nullptr;
	}

	void DrawTextItem(UPARAM(ref) FCanvas*& Canvas, const FVector2D& AtLocation, const FString& InSuffix, const FString& InString);
	void DrawTextureItem(UPARAM(ref) FCanvas*& Canvas, const int32& Width, const int32& Height, UTexture2D* Texture2D);

private:

	UTexture2D* GetTextureFromGeneratedClass(UClass* Class) const;

};
