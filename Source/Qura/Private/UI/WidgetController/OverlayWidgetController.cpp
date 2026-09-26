#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    if (GetAuraASC())
    {
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