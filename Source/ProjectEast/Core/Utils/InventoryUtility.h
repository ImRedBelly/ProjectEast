#pragma once

class UInteractableComponent;

class InventoryUtility
{
public:
	static UInteractableComponent* GetCurrentInteractableObject(AActor* OwningPlayer);
	
};
