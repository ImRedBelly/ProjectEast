#pragma once

#include "CoreMinimal.h"
#include "CraftingCore.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "PlayerCrafting.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerCrafting : public UCraftingCore
{
	GENERATED_BODY()
	
public:
	void OpenCraftingWidget(UCraftingCore* CraftingCore, EWidgetType WidgetType);
	void CloseCraftingWidget();
	void InitializeCrafting(AMainPlayerController* PlayerController);
	void ClientInitializeCrafting(AMainPlayerController* PlayerController);

protected:
	virtual void BeginPlay() override;
	
private:

	UPROPERTY()
	UCraftingCore* CurrentStationComponent;
	UPROPERTY()
	AMainPlayerController* PlayerController;
	UPROPERTY()
	UWidgetManager* WidgetManager;

	bool bIsCraftingWidgetOpen;
};
