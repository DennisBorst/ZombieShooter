#include "GameMode_Main.h"

#include "GameInstance_Main.h"
#include "SessionSubsystem_Main.h"

AGameMode_Main::AGameMode_Main()
{
	bUseSeamlessTravel = true;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "INIT GAMEMODE");
}

void AGameMode_Main::BeginPlay()
{
	Super::BeginPlay();
}

void AGameMode_Main::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
		
	PlayerCharacters.Add(NewPlayer);
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, NewPlayer->GetName());
}

void AGameMode_Main::Logout(AController* ExitingPlayer)
{
	Super::Logout(ExitingPlayer);

	if (PlayerCharacters.Contains(ExitingPlayer))
		PlayerCharacters.Remove(ExitingPlayer);
}

APawn* AGameMode_Main::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	APawn* TempPawn = GetWorld()->SpawnActor<APawn>(APawn::StaticClass(), FTransform(FRotator::ZeroRotator, FVector::Zero(), FVector::One()));
	return TempPawn;
}

EZombieGameState AGameMode_Main::SetGameState(EZombieGameState newState)
{
	CurrentGameState = newState;
	HandleGameState();
	return newState;
}

EZombieGameState AGameMode_Main::GetGameState() const
{
	return CurrentGameState;
}

void AGameMode_Main::HandleGameState()
{
	switch (CurrentGameState)
	{
	case EZombieGameState::DEFAULT:
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "DEFAULT STATE");
		break;
	case EZombieGameState::PRE_LOBBY:
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "PRE LOBBY STATE");

		SetGameState(EZombieGameState::LOBBY);

		break;
	case EZombieGameState::LOBBY:
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "LOBBY STATE");
		break;
	case EZombieGameState::PRE_GAME:
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "INGAME STATE");

		SetGameState(EZombieGameState::INGAME);

		break;
	case EZombieGameState::INGAME:
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "INGAME STATE");
		break;
	case EZombieGameState::POSTGAME:
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "POSTGAME STATE");
		break;
	default:
		break;
	}
}

void AGameMode_Main::SpawnPlayer(AController* PlayerToSpawn)
{
	
}

void AGameMode_Main::RespawnPlayer(AController* PlayerToRespawn)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, PlayerToRespawn->GetName());
}

void AGameMode_Main::StartLobby(bool isLANSession)
{
	if (UKismetSystemLibrary::IsServer(GetWorld())) {
		SetGameState(EZombieGameState::PRE_LOBBY);

		const FString URL = LobbyLevel.ToString() + "?Listen";
		GetWorld()->ServerTravel(URL, true, false);

		UGameInstance_Main* GameInstance = Cast<UGameInstance_Main>(UGameplayStatics::GetGameInstance(GetWorld()));
		USessionSubsystem_Main* SessionSubsystem = GameInstance->GetSubsystem<USessionSubsystem_Main>();
		SessionSubsystem->CreateSession(4, isLANSession);
	}
}

void AGameMode_Main::StartGame()
{
	if (UKismetSystemLibrary::IsServer(GetWorld())) {
		SetGameState(EZombieGameState::PRE_GAME);

		const FString URL = GameLevel.ToString() + "?Listen";
		GetWorld()->ServerTravel(URL, false, false);
	}
}


