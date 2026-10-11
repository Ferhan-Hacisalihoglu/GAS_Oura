#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
    LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
    LoadSlot_0->SlotIndex = 0;
    LoadSlots.Add(0, LoadSlot_0);
    LoadSlot_0->InitializeSlot();

    LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
    LoadSlot_1->SlotIndex = 1;
    LoadSlots.Add(1, LoadSlot_1);
    LoadSlot_1->InitializeSlot();

    LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
    LoadSlot_2->SlotIndex = 2;
    LoadSlots.Add(2, LoadSlot_2);
    LoadSlot_2->InitializeSlot();
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
    return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::SetNumLoadSlots(int32 InNumLoadSlots)
{
    UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlots, InNumLoadSlots);
}

void UMVVM_LoadScreen::LoadData()
{
    LoadSlot_0->SetPlayerName(TEXT("Player1"));
    LoadSlot_0->SlotStatus = Vacant;
    LoadSlot_0->SetMapName(TEXT("Map1"));
    LoadSlot_0->SetPlayerLevel(1);
    LoadSlot_0->InitializeSlot();
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
    LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
    LoadSlots[Slot]->SetPlayerName(EnteredName);
    LoadSlots[Slot]->SetPlayerLevel(1);
    LoadSlots[Slot]->SlotStatus = Taken;

    LoadSlots[Slot]->InitializeSlot();
}
