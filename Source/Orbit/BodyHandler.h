// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "Body.h"
#include "OcNode.h"
#include "GameFramework/Actor.h"
#include "BodyHandler.generated.h"

UCLASS()
class ORBIT_API ABodyHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABodyHandler();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	std::vector<ABody*> Bodies;

	OcNode *ocNode;
	
};
