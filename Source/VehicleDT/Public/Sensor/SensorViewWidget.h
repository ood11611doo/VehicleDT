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
	/** 카메라 렌더 타겟을 SensorImage에 연결 → 카메라 뷰 표시 */
	UFUNCTION(BlueprintCallable, Category = "SensorView")
	void SetRenderTarget(UTextureRenderTarget2D* InRenderTarget);

	/** BEV 텍스처를 LidarImage에 연결 → BEV 뷰 표시 */
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
	/** 카메라 뷰 컨테이너 — Collapsed/SelfHitTestInvisible 전환으로 켜고 끔 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SensorBorder;

	/** 카메라 렌더 타겟 표시 위젯 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SensorImage;

	/** BEV 뷰 컨테이너 — Collapsed/SelfHitTestInvisible 전환으로 켜고 끔 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> LidarBorder;

	/** BEV 텍스처 표시 위젯 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LidarImage;
};
