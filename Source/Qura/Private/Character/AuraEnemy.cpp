#include "Character/AuraEnemy.h"
#include "AI/AuraAIController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/Widget/AuraUserWidget.h"
#include "Components/WidgetComponent.h"

AAuraEnemy::AAuraEnemy()
{
    PrimaryActorTick.bCanEverTick = false;

    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));

    HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::BeginPlay()
{
    Super::BeginPlay();

    for (TSubclassOf<UGameplayAbility> AbilityClass : CommonAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
        AbilitySystemComponent->GiveAbility(AbilitySpec);
    }

    FGameplayEffectContextHandle VitalAttributesContextHandle = AbilitySystemComponent->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AbilitySystemComponent->GetAvatarActor());
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultVitalAttributes, 1.f, VitalAttributesContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());

    if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
    {
        AuraUserWidget->SetWidgetController(this);
    }

    if(const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                OnHealthChanged.Broadcast(Data.NewValue);
            }
        );

        OnHealthChanged.Broadcast(AuraAS->GetHealth());
    }

}

void AAuraEnemy::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (!HasAuthority())
    {
        return;
    }
    
    AuraAIController = Cast<AAuraAIController>(NewController);
    AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    AuraAIController->RunBehaviorTree(BehaviorTree);
}

void AAuraEnemy::HighlightActor_Implementation()
{
    GetMesh()->SetRenderCustomDepth(true);
    Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnhighlightActor_Implementation()
{
    GetMesh()->SetRenderCustomDepth(false);
    Weapon->SetRenderCustomDepth(false);
}