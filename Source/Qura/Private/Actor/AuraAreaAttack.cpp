#include "Actor/AuraAreaAttack.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Qura/Qura.h"

AAuraAreaAttack::AAuraAreaAttack()
{
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AuraEffectComponent = CreateDefaultSubobject<UNiagaraComponent>("AuraEffectComponent");
	AuraEffectComponent->SetupAttachment(Sphere);
	AuraEffectComponent->bAutoActivate = true;
}

void AAuraAreaAttack::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraAreaAttack::OnSphereBeginOverlap);

	SetLifeSpan(3.f);
}

void AAuraAreaAttack::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if (!HasAuthority() || !IsValidOverlap(OtherActor) || DamagedActors.Contains(OtherActor))
	{
		return;
	}

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
		UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		DamagedActors.Add(OtherActor);
	}
}

bool AAuraAreaAttack::IsValidOverlap(AActor* OtherActor) const
{
	if (!IsValid(OtherActor) || DamageEffectParams.SourceAbilitySystemComponent == nullptr)
	{
		return false;
	}

	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	return SourceAvatarActor != OtherActor && UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor, OtherActor);
}
