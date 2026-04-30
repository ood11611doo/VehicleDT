// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SensorViewWidget.generated.h"

class UImage;
class UBorder;
class UTextureRenderTarget2D;

UCLASS()
class VEHICLEDT_API USensorViewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SensorView")
	void SetRenderTarget(UTextureRenderTarget2D* InRenderTarget);

	UFUNCTION(BlueprintCallable, Category = "SensorView")
	void SetLidarRenderTarget(UTexture2D* InRenderTarget);

	UFUNCTION(BlueprintCallable, Category = "SensorView")
	void ToggleCameraView();

	UFUNCTION(BlueprintCallable, Category = "SensorView")
	void ToggleLidarView();

	UFUNCTION(BlueprintPure, Category = "SensorView")
	bool IsCameraViewVisible() const;

	UFUNCTION(BlueprintPure, Category = "SensorView")
	bool IsLidarViewVisible() const;

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SensorBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SensorImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> LidarBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LidarImage;
};
