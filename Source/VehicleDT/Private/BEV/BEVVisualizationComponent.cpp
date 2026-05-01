// Fill out your copyright notice in the Description page of Project Settings.

#include "BEV/BEVVisualizationComponent.h"
#include "Engine/Texture2D.h"
#include "Sensor/LidarSensorComponent.h"


UBEVVisualizationComponent::UBEVVisualizationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBEVVisualizationComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//TODO : QHD나 4k에서는 512*512가 작아보이면? 19~23줄 추가. 이거 아님 말고 앵커 수정 ㄱ
	
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	
	const int32 Desired = FMath::RoundToInt(FMath::Min(ViewportSize.X, ViewportSize.Y)* 0.25f);
	Config.ImageSize = FMath::RoundUpToPowerOfTwo(Desired);
	
	CreateTexture();
	BuildColorLUT();
	
	if (auto* Lidar = GetOwner()->FindComponentByClass<ULidarSensorComponent>())
	{
		Lidar->OnLidarScanReady.AddUObject(this, &UBEVVisualizationComponent::HandleLidarScan);
	}
}

void UBEVVisualizationComponent::HandleLidarScan(const FLidarPointCloudData& PointCloud)
{
	if (!DynamicTexture) return;
	UpdatePixelBuffer(PointCloud);
}

void UBEVVisualizationComponent::CreateTexture()
{
	const int32 N = Config.ImageSize;

	// GPU 텍스쳐 메모리 할당, PF_B8G8R8A8 - BGRA순서
	DynamicTexture = UTexture2D::CreateTransient(N, N, PF_B8G8R8A8);
	//픽셀 선명하게
	DynamicTexture->Filter = TF_Nearest;
	DynamicTexture->SRGB = true;
	DynamicTexture->UpdateResource();

	// CPU 픽셀 버퍼 확보
	PixelBuffer.SetNumUninitialized(N * N);
	// GPU 업로드 메타 정보 초기화
	UpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, N, N);
}

void UBEVVisualizationComponent::BuildColorLUT()
{
	for (int32 i = 0; i < 256; i++)
	{
		const float T = i / 255.f;
		ColorLUT[i] = FLinearColor::LerpUsingHSV(
			FLinearColor::Blue, FLinearColor::Red, T).ToFColor(true);
	}
}

void UBEVVisualizationComponent::UpdatePixelBuffer(const FLidarPointCloudData& PointCloud)
{
	/**
	 * Scale = (ImageSize / 2) / ViewRange - Pixel per Cm
	 * PixelX = HalfSize + LocalY * Scale - UE Y 축 (우측) -> 화면 X 축
	 * PixelY = HalfSize - LocalX * Scale - UE X 축(전방) ->  화면 Y 축 위족
	 * 
	 * UE 좌표계  : X = 전방, Y = 우측, Z 위
	 * 화면 좌표계 : X = 오른쪽 , Y = 아래
	 * 
	 * 차의 전방이 화면 위로 가야함. 예를 들면 네비게이션
	 * X -> Pixel로 매핑하고 부호 반전
	 */
	
	if (!DynamicTexture) return;
		
	const int32 N = Config.ImageSize;
	const float Scale = (N * 0.5f) / Config.ViewRange;
	const float Half = N * 0.5f;


	// 1. 백그라운드 채우기
	const FColor Bgcolor = Config.BackgroundColor.ToFColor(true);
	for (FColor& P : PixelBuffer) P = Bgcolor;

	if (bShowGrid) DrawGrid();
	if (bShowHeading) DrawHeading();
	
	// 2. 차량기준 로컬 변환
	const FTransform Inv =GetOwner()->GetActorTransform().Inverse();
	const int32 PS = FMath::Max((int32)Config.PointSize, 1);
	const int32 PH = PS / 2;
	
	for (int32 i = 0; i < PointCloud.PointCount; ++i)
	{
		//월드 좌표를 차량 로컬 좌표로 변환. 이게 안되면 차량이 움직여도 포인트는 멈춰있음
		const FVector Local = Inv.TransformPosition(PointCloud.Points[i]);
		
		const int32 PX = FMath::RoundToInt32(Half + Local.Y * Scale);
		const int32 PY = FMath::RoundToInt32(Half - Local.X * Scale);
		
		if (PX < PH || PX >= N -PH || PY < PH || PY >= N -PH) continue;
		
		const FColor Color = MakeColorFromHeight(Local.Z);
		
		for (int32 dy = -PH; dy <= PH; ++dy)
		for (int32 dx = -PH; dx <= PH; ++dx)
		    PixelBuffer[(PY + dy) * N + (PX + dx)] =Color;
	}

	// 3. CPU -> GPU
	DynamicTexture->UpdateTextureRegions(
		0, 1, &UpdateRegion,
		N * sizeof(FColor), sizeof(FColor),
		reinterpret_cast<uint8*>(PixelBuffer.GetData()));
}

void UBEVVisualizationComponent::DrawGrid()
{
	const int32 N    = Config.ImageSize;
	const float Half = N * 0.5f;
	const float Scale = Half / Config.ViewRange;
	const float Step = GridSpacingMeters * 100.f * Scale;
	const FColor C(150, 150, 150, 255);

	for (float Off = 0; Off < Half; Off += Step)
	{
		const int32 R = FMath::RoundToInt32(Half + Off);
		const int32 L = FMath::RoundToInt32(Half - Off);
		for (int32 i = 0; i < N; ++i)
		{
			if (R < N) { PixelBuffer[i * N + R] = C; PixelBuffer[R * N + i] = C; }
			if (L >= 0){ PixelBuffer[i * N + L] = C; PixelBuffer[L * N + i] = C; }
		}
	}
	OnBEVUpdate.Broadcast(DynamicTexture);
}

void UBEVVisualizationComponent::DrawHeading()
{
	const int32 N = Config.ImageSize;
	const int32 C = N / 2;
	const FColor Body(255, 100, 0, 255);
	
	for (int32 i = 0; i < 12; ++i)
		for (int32 w = -1; w<=1; ++w)
			PixelBuffer[(C - i) * N + (C + w)] = Body;
	
	for (int32 i = 0; i <6; ++i)
		for (int32 w = -i; w<=i; ++w)
			PixelBuffer[(C- 12 + i) * N + (C + w)] = Body;
}

FColor UBEVVisualizationComponent::MakeColorFromHeight(float Height) const
{
	const float T = FMath::GetMappedRangeValueClamped(
		FVector2f(HeightMinCm, HeightMaxCm),
		FVector2f(0.f, 1.f), Height);

	// 계산없이 ColorLut에서 빠르게 조회만 하게
	return ColorLUT[static_cast<uint8>(T * 255.f)];
}
