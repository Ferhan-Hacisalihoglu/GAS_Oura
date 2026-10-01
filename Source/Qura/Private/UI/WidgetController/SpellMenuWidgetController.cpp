#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
    BroadcastAbilityInfo();
    SpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
    GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
    {
        if(SelectedAbility.Ability.MatchesTagExact(AbilityTag))
        {
            SelectedAbility.Status = StatusTag;
            bool bEnableSpendPoints = false;
            bool bEnabledEquip = false;
            ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnabledEquip);
            SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnabledEquip);
        }

       if (AbilityInfo)
       {
            FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
            Info.StatusTag = StatusTag;
            AbilityInfoDelegate.Broadcast(Info);
       } 
    });

    GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

    GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoints)
    {
        SpellPointsChanged.Broadcast(SpellPoints);
        CurrentSpellPoints = SpellPoints;

        bool bEnableSpendPoints = false;
        bool bEnabledEquip = false;
        ShouldEnableButtons(SelectedAbility.Status, SpellPoints, bEnableSpendPoints, bEnabledEquip);
        SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnabledEquip); 
    });

}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
    if (bWaitingForEquipSelection)
    {
        StopWaitingForEquipDelegate.Broadcast();
        bWaitingForEquipSelection = false;
    }

    FGameplayTag AbilityStatus;
    const int32 SpellPoints = GetAuraPS()->GetSpellPoints();

    const bool bTagValid = AbilityTag.IsValid();
    const bool bTagNone = AbilityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.None")));
    const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
    const bool bSpecValid = AbilitySpec != nullptr;

    if (!bTagValid || bTagNone || !bSpecValid)
    {
        AbilityStatus = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Locked"));
    }
    else
    {
        AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
    }

    SelectedAbility.Ability = AbilityTag;
    SelectedAbility.Status = AbilityStatus;

    bool bEnableSpendPoints = false;
    bool bEnabledEquip = false;
    ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnabledEquip);
    SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnabledEquip);
}   


void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
    if (AbilityStatus.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped"))))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Eligible"))))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked"))))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
    if(!bWaitingForEquipSelection)  return;
    GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
    FAuraAbilityInfo LastSlotInfo;
    LastSlotInfo.InputTag = PreviousSlot;
    LastSlotInfo.StatusTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked"));
    LastSlotInfo.AbilityTag = FGameplayTag::RequestGameplayTag(FName("Abilities.None"));
    AbilityInfoDelegate.Broadcast(LastSlotInfo);

    FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
    Info.InputTag = Slot;
    Info.AbilityTag = Status;
    AbilityInfoDelegate.Broadcast(Info);

    StopWaitingForEquipDelegate.Broadcast();
    SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
    GlobeDeselected();
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
    if(GetAuraASC())
    {
        GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
    }
}

void USpellMenuWidgetController::EquipButtonPressed()
{
    WaitForEquipDelegate.Broadcast();
    bWaitingForEquipSelection = true;
}

void USpellMenuWidgetController::GlobeDeselected()
{
    if (bWaitingForEquipSelection)
    {
        StopWaitingForEquipDelegate.Broadcast();
        bWaitingForEquipSelection = false;
    }

    SelectedAbility.Ability = FGameplayTag::RequestGameplayTag(FName("Abilities.None"));
    SelectedAbility.Status = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Locked"));

    SpellGlobeSelectedDelegate.Broadcast(false, false);
}
