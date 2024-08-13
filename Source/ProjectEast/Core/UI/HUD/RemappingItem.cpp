#include "RemappingItem.h"

void URemappingItem::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeItem();

	InputKey->OnKeySelected.AddDynamic(this, &URemappingItem::KeySelected);
	InputKey->OnIsSelectingKeyChanged.AddDynamic(this, &URemappingItem::IsSelectingKeyChanged);
}

void URemappingItem::NativeDestruct()
{
	Super::NativeDestruct();
	InputKey->OnKeySelected.RemoveDynamic(this, &URemappingItem::KeySelected);
	InputKey->OnIsSelectingKeyChanged.RemoveDynamic(this, &URemappingItem::IsSelectingKeyChanged);
}


void URemappingItem::KeySelected(FInputChord Key)
{
	if (bIsListenForRemap)
	{
		bIsListenForRemap = false;
		MappingContext->UnmapKey(InputAction, CurrentKey);
		MappingContext->MapKey(InputAction, Key.Key);
		CurrentKey = Key.Key;
		OnChangeIcon();
	}
}

void URemappingItem::IsSelectingKeyChanged()
{
	bIsListenForRemap = true;
	ImageInput->SetBrush(EmptyViewIcon);
}

void URemappingItem::InitializeItem()
{
	for (auto Mapping : MappingContext->GetMappings())
	{
		if (Mapping.Action == InputAction)
		{
			FInputChord InSelectedKey(Mapping.Key);
			InputKey->SetSelectedKey(InSelectedKey);

			CurrentKey = Mapping.Key;
			OnChangeIcon();
			break;
		}
	}
}

void URemappingItem::OnChangeIcon()
{
	// for (FName RowName : DataTableKeys->GetRowNames())
	// {
	// 	auto DataTableRowInfo = DataTableKeys->FindRow<FKeyIconData>(RowName, TEXT(""));
	// 	if (DataTableRowInfo->Key == CurrentKey)
	// 	{
	// 		SetIconKey(DataTableRowInfo->IconKey);
	// 		break;
	// 	}
	// }

	for (FName RowName : DataTableKeys->GetRowNames())
	{
		auto DataTableRowInfo = DataTableKeys->FindRow<FKeyIconsData>(RowName, TEXT(""));
		if (DataTableRowInfo->IconInfo.Contains(CurrentKey))
		{
			SetIconKey(DataTableRowInfo->IconInfo[CurrentKey]);
			break;
		}
	}
}


void URemappingItem::SetIconKey(UTexture2D* IconKey) const
{
	ImageInput->SetBrushFromTexture(IconKey);
}
