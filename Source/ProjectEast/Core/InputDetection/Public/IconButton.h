#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Components/Widget.h"
#include "Engine/StreamableManager.h"
#include "IconButton.generated.h"


UCLASS()
class PROJECTEAST_API UIconButton : public UWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_RetVal(FName, FGetName);

public:
	UPROPERTY()
	FSlateBrush Brush;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FName InputBinding;

	UPROPERTY()
	FGetName InputBindingDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=(sRGB = "true"))
	FLinearColor ColorAndOpacity;

	UPROPERTY()
	FGetLinearColor ColorAndOpacityDelegate;

	UPROPERTY(EditAnywhere, Category=Localization)
	bool bFlipRightToLeftFlowDirection;

	UPROPERTY(EditAnywhere, Category=PLATFORM_IMPLEMENTS_BeginNamedEventStatic, meta=(IsBindableEvent = "true"))
	FOnPointerEvent OnMouseButtonDown;

	FDelegateHandle InputChangedDelegate;


	UIconButton();
	void Initialize(UInputAction* InputAction);

	UFUNCTION(BlueprintCallable, Category=Appearance)
	void SetInputBinding(FName InBinding);

	UFUNCTION(BlueprintCallable, Category=Appearance)
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category=Appearance)
	void SetOpacity(float InOpacity);
	
	
	void OnInputTypeChanged();
	void SetBrushFromTexture(UTexture2D* Texture);
	void SetBrushFromSoftTexture(TSoftObjectPtr<UTexture2D> SoftTexture);
	void RequestAsyncLoad(TSoftObjectPtr<UObject> SoftObject, TFunction<void()>&& Callback);
	virtual void RequestAsyncLoad(TSoftObjectPtr<UObject> SoftObject, FStreamableDelegate DelegateToCall);
	virtual void CancelImageStreaming();

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void SynchronizeProperties() override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	TSharedPtr<SImage> ImageWidget;
	TSharedPtr<FStreamableHandle> StreamingHandle;
	FSoftObjectPath StreamingObjectPath;

	PROPERTY_BINDING_IMPLEMENTATION(FName, InputBinding);
	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, ColorAndOpacity);
	PROPERTY_BINDING_IMPLEMENTATION(bool, bIsEnabled);

	FReply HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent);

	virtual TSharedRef<SWidget> RebuildWidget() override;

#if WITH_ACCESSIBILITY
	virtual TSharedPtr<SWidget> GetAccessibleWidget() const override;
#endif
	

	UPROPERTY(EditAnywhere)
	UInputMappingContext* CurrentMappingContext;
	UPROPERTY(EditAnywhere)
	UInputAction* CurrentInputAction;
};
