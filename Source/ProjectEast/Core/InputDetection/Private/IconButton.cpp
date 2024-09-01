#include "ProjectEast/Core/InputDetection/Public/IconButton.h"
#include "Engine/AssetManager.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/Utils/GameTypes.h"

#define LOCTEXT_NAMESPACE "ButtonWidget"

class FIconButtonGameModule;

UIconButton::UIconButton() :
	InputBinding(NAME_None),
	ColorAndOpacity(FLinearColor::White)
{
}

void UIconButton::Initialize(UInputAction* InputAction)
{
	CurrentInputAction = InputAction;
	OnInputTypeChanged();
}

void UIconButton::SetInputBinding(FName InBinding)
{
	InputBinding = InBinding;
	if (ImageWidget.IsValid())
	{
		OnInputTypeChanged();
	}
}

void UIconButton::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;
	if (ImageWidget.IsValid())
	{
		ImageWidget->SetColorAndOpacity(ColorAndOpacity);
	}
}

void UIconButton::SetOpacity(float InOpacity)
{
	ColorAndOpacity.A = InOpacity;
	if (ImageWidget.IsValid())
	{
		ImageWidget->SetColorAndOpacity(ColorAndOpacity);
	}
}

void UIconButton::OnInputTypeChanged()
{
	Brush.SetResourceObject(nullptr);
	if (ImageWidget.IsValid() && IsValid(CurrentMappingContext) && IsValid(CurrentInputAction))
	{
		auto& GameModule = FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);
		auto Texture = GameModule.GetTextureForKey(CurrentMappingContext, CurrentInputAction);
		if (!Texture.IsNull())
			SetBrushFromSoftTexture(Texture);
	}
}

void UIconButton::SetBrushFromTexture(UTexture2D* Texture)
{
	if (Brush.GetResourceObject() != Texture)
	{
		Brush.SetResourceObject(Texture);
		if (Texture)
		{
			Texture->bForceMiplevelsToBeResident = true;
			Texture->bIgnoreStreamingMipBias = true;
			Brush.ImageSize.X = Texture->GetSizeX();
			Brush.ImageSize.Y = Texture->GetSizeY();
		}
		if (ImageWidget.IsValid())
		{
			ImageWidget->SetImage(&Brush);
		}
	}
}

void UIconButton::SetBrushFromSoftTexture(TSoftObjectPtr<UTexture2D> SoftTexture)
{
	TWeakObjectPtr<UIconButton> WeakThis(this);
	RequestAsyncLoad(SoftTexture, [WeakThis, SoftTexture]()
	{
		if (UIconButton* StrongThis = WeakThis.Get())
		{
			ensureMsgf(SoftTexture.Get(), TEXT("Failed to load &s"), *SoftTexture.ToSoftObjectPath().ToString());
			StrongThis->SetBrushFromTexture(SoftTexture.Get());
		}
	});
}

void UIconButton::RequestAsyncLoad(TSoftObjectPtr<UObject> SoftObject, TFunction<void()>&& Callback)
{
	RequestAsyncLoad(SoftObject, FStreamableDelegate::CreateLambda(MoveTemp(Callback)));
}

void UIconButton::RequestAsyncLoad(TSoftObjectPtr<UObject> SoftObject, FStreamableDelegate DelegateToCall)
{
	CancelImageStreaming();
	if (UObject* StrongObject = SoftObject.Get())
	{
		DelegateToCall.ExecuteIfBound();
		return;
	}

	TWeakObjectPtr<UIconButton> WeakThis(this);
	StreamingObjectPath = SoftObject.ToSoftObjectPath();
	StreamingHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
		StreamingObjectPath, [WeakThis, DelegateToCall, SoftObject]()
		{
			if (UIconButton* StrongThis = WeakThis.Get())
			{
				if (StrongThis->StreamingObjectPath != SoftObject.ToSoftObjectPath())
					return;

				DelegateToCall.ExecuteIfBound();
			}
		}, FStreamableManager::AsyncLoadHighPriority
	);
}

void UIconButton::CancelImageStreaming()
{
	if (StreamingHandle.IsValid())
	{
		StreamingHandle->CancelHandle();
		StreamingHandle.Reset();
	}
	StreamingObjectPath.Reset();
}

void UIconButton::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	ImageWidget.Reset();
}

void UIconButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!InputChangedDelegate.IsValid())
	{
		auto& GameModule = FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);
		InputChangedDelegate = GameModule.InputDeviceChangedDelegate.AddUObject(this,&UIconButton::OnInputTypeChanged);
	}

	TAttribute<FName> InputBindingBinding = PROPERTY_BINDING(FName, InputBinding);
	TAttribute<FSlateColor> ColorAndOpacityBinding = PROPERTY_BINDING(FSlateColor, ColorAndOpacity);

	if (ImageWidget.IsValid())
	{
		this->SetInputBinding(InputBinding);
		ImageWidget->SetImage(&Brush);
		ImageWidget->SetColorAndOpacity(ColorAndOpacityBinding);
		ImageWidget->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDown));
	}
}

#if WITH_EDITOR
const FText UIconButton::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}
#endif
FReply UIconButton::HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (OnMouseButtonDown.IsBound())
		return OnMouseButtonDown.Execute(Geometry, MouseEvent).NativeReply;
	return FReply::Unhandled();
}

TSharedRef<SWidget> UIconButton::RebuildWidget()
{
	ImageWidget = SNew(SImage).FlipForRightToLeftFlowDirection(bFlipRightToLeftFlowDirection);
	return ImageWidget.ToSharedRef();
}

#if WITH_ACCESSIBILITY
TSharedPtr<SWidget> UIconButton::GetAccessibleWidget() const
{
	return ImageWidget;
}
#endif

#undef LOCTEXT_NAMESPACE
