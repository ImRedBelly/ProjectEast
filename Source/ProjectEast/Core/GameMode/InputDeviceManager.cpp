#include "InputDeviceManager.h"
#include "ProjectEast/Core/UI/Menus/RemappingItem.h"

UTexture2D* UInputDeviceManager::GetIconKeyByActionType(UInputAction* InputAction) const
{
	for (auto Mapping : MappingContext->GetMappings())
	{
		if (Mapping.Action == InputAction)
		{
			for (FName RowName : DataTableKeys->GetRowNames())
			{
				auto DataTableRowInfo = DataTableKeys->FindRow<FKeyIconsData>(RowName, TEXT(""));
				if (DataTableRowInfo->IconInfo.Contains(Mapping.Key))
					return DataTableRowInfo->IconInfo[Mapping.Key];
			}
		}
	}

	return nullptr;
}
