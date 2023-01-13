// Project 'Zombie Shooter' -Tim Peeters 2022/23

#include "Characters/Player/Weapons/WeaponPickupPoint.h"

AWeaponPickupPoint::AWeaponPickupPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

void AWeaponPickupPoint::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeaponPickup();
}

void AWeaponPickupPoint::Tick(float Delta)
{
	Super::Tick(Delta);
	if (CurrentWeaponObject) {
		CurrentWeaponObject->AddActorWorldRotation((FRotator(0, MeshRotationSpeed, 0) * Delta));
	}
}

float AWeaponPickupPoint::GenerateSpawnDelay()
{
	return FMath::RandRange(MinSpawnDelay, MaxSpawnDelay);
}

void AWeaponPickupPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponPickupPoint, CurrentWeaponObject);
}

void AWeaponPickupPoint::SpawnWeaponPickup()
{
	if (!UKismetSystemLibrary::IsServer(GetWorld())) return;

	if (!SpawnableWeaponTypes.IsEmpty()) {

		FTransform SpawnTransform = FTransform(GetActorRotation(), GetActorLocation() + FVector(0, 0, 90.0f), FVector::OneVector);
		AWeaponObject* NewWeaponObject = (AWeaponObject*)GetWorld()->SpawnActorDeferred<AWeaponObject>(AWeaponObject::StaticClass(),
			SpawnTransform);

		NewWeaponObject->SetReplicates(true);

		uint8 WeaponObjectIndex = FMath::RandRange(0, (SpawnableWeaponTypes.Num() - 1));
		NewWeaponObject->WeaponData = SpawnableWeaponTypes[WeaponObjectIndex];

		UGameplayStatics::FinishSpawningActor(NewWeaponObject, NewWeaponObject->GetTransform());

		NewWeaponObject->OnWeaponPickup.AddDynamic(this, &AWeaponPickupPoint::OnPickup);
		CurrentWeaponObject = NewWeaponObject;

		GetWorld()->GetTimerManager().PauseTimer(SpawnTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

void AWeaponPickupPoint::OnPickup()
{
	if (CurrentWeaponObject) {
		CurrentWeaponObject->OnWeaponPickup.RemoveDynamic(this, &AWeaponPickupPoint::OnPickup);
		CurrentWeaponObject = nullptr;

		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AWeaponPickupPoint::SpawnWeaponPickup, GenerateSpawnDelay(), false, -1.0f);
	}
}






