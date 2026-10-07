#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
    OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
    OnManaChanged.Broadcast(GetAuraAS()->GetMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                OnHealthChanged.Broadcast(Data.NewValue);
            }
        );

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);

    if (GetAuraASC())
    {
        GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);

        if (GetAuraASC()->bStartupAbilitiesGiven)
        {
            BroadcastAbilityInfo();
        }
        else 
        {
            GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
        }

        GetAuraASC()->EffectAssetTags.AddLambda(
            [this](const FGameplayTagContainer& AssetTags)
            {
                for (const FGameplayTag& Tag : AssetTags)
                {
                    // For example, say that Tag = Message.HealthPosition
                    // "Messsage.HealthPosition".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPosition") will return False
                    FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
                    if (Tag.MatchesTag(MessageTag))
                    {
                        // GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Blue,TEXT("Getted GameplayTag info!"));
                        const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
                        if  (Row)
                        {   
                            MessageWidgetRowDelegate.Broadcast(*Row);
                        }
                    }
                }
            }
        );
    }
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
    FAuraAbilityInfo LastSlotInfo;
    LastSlotInfo.StatusTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked"));
    LastSlotInfo.InputTag = PreviousSlot;
    LastSlotInfo.AbilityTag = FGameplayTag::RequestGameplayTag(FName("Abilities.None"));
    AbilityInfoDelegate.Broadcast(LastSlotInfo);

    FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
    Info.StatusTag = Status;
    Info.InputTag = Slot;
    AbilityInfoDelegate.Broadcast(Info);
}