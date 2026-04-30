// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sensor/SensorTypes.h"
#include "AgentDataLoggerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEHICLEDT_API UAgentDataLoggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAgentDataLoggerComponent();

	UFUNCTION(BlueprintCallable, Category = "DataLogger")
	void StartRecording();

	UFUNCTION(BlueprintCallable, Category = "DataLogger")
	void StopRecording();

	UFUNCTION(BlueprintPure, Category = "DataLogger")
	bool IsRecording() const { return bIsRecording; }

	void HandleLidarScan(const FLidarPointCloudData& PointCloud);
	void HandleCameraFrame();
	void HandleControlOutput(float Value);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:
	static int32 GetUtmZone(double Longitude);
	static void LatLonToUtm(double Lat, double Lon, int32 Zone, double& OutEasting, double& OutNorthing);
	void WorldToUtm(const FVector& WorldLocation, double& OutEasting, double& OutNorthing) const;
	void CreateCsvFile();
	void AppendRow();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataLogger",
		meta=(AllowPrivateAccess="true"))
	bool bEnableLogging = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataLogger",
		meta=(ClampMin="0.1", ClampMax="100.0", Units="Hz", AllowPrivateAccess="true"))
	float SaveFrequencyHz = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataLogger|UTM Reference",
		meta=(ClampMin="-90.0", ClampMax="90.0", Units="deg", AllowPrivateAccess="true"))
	double OriginLatitude = 36.4800;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataLogger|UTM Reference",
		meta=(ClampMin="-180.0", ClampMax="180.0", Units="deg", AllowPrivateAccess="true"))
	double OriginLongitude = 127.0000;

private:
	double OriginUtmEasting = 0.0;
	double OriginUtmNorthing = 0.0;
	int32 OriginUtmZone = 0;

	FString CsvFilePath;
	bool bIsRecording = false;
	float TimeSinceLastSave = 0.0f;
	float ElapsedRecordingTime = 0.0f;
};
