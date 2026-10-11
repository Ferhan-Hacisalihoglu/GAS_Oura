#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/LoadScreenSaveGame.h"

void AAuraGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    Maps.Add(DefaultMapName, DefaultMap);
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
    const FString SlotName = Slot->GetLoadSlotName();
    const int32 SlotIndex = Slot->SlotIndex;

    UGameplayStatics::OpenLevelBySoftObjectPtr(Slot,Maps.FindChecked(Slot->GetMapName()));
}

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
    if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(), SlotIndex))
    {
        UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(), SlotIndex);
    }
    USaveGame* SaveGameInstance = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
    ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameInstance);
    LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
    LoadScreenSaveGame->SaveSlotStatus = Taken;
    LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
    LoadScreenSaveGame->LoadSlotName = LoadSlot->GetLoadSlotName();

    UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
    USaveGame* SaveGameObject = nullptr;
    if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
    {
        SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
    }
    else
    {
        SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
    }   
    ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
    return LoadScreenSaveGame;
}

void AAuraGameModeBase::DeleteSlot(const FString& LoadSlot, int32 SlotIndex)
{
    if (UGameplayStatics::DoesSaveGameExist(LoadSlot, SlotIndex))
    {
        UGameplayStatics::DeleteGameInSlot(LoadSlot, SlotIndex);
    }
}
