// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <improbable/c_worker.h>
#include <string>
#include "SpatialPlayerSpawner.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpatialGDKPlayerSpawner, Log, All);

class USpatialNetDriver;

UCLASS()
class SPATIALGDK_API USpatialPlayerSpawner : public UObject
{
	GENERATED_BODY()
	
public:

	void Init(USpatialNetDriver* NetDriver, FTimerManager* TimerManager);

	// Server
	void ReceivePlayerSpawnRequest(std::string& URL, Worker_RequestId RequestId );

	// Client
	void SendPlayerSpawnRequest();
	void ReceivePlayerSpawnResponse();

private:
	USpatialNetDriver* NetDriver;
	
	FTimerManager* TimerManager;
	int NumberOfAttempts;
};
