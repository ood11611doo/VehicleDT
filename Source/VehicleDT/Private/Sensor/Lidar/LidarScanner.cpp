#include "Sensor/Lidar/LidarScanner.h"
#include "Engine/World.h"

static const TArray<float> ElevTable_VLP16 = {
	-15.f, -13.f, -11.f, -9.f, -7.f, -5.f, -3.f, -1.f,
	  1.f,   3.f,   5.f,  7.f,  9.f, 11.f, 13.f, 15.f
};

static const TArray<float> ElevTable_VLP32C = {
	-25.00f, -15.64f, -11.31f,  -8.84f, -7.25f, -6.15f, -5.33f, -4.67f,
	 -4.00f,  -3.67f,  -3.33f,  -3.00f, -2.67f, -2.33f, -2.00f, -1.67f,
	 -1.33f,  -1.00f,  -0.67f,  -0.33f,  0.00f,  0.33f,  0.67f,  1.00f,
	  1.33f,   1.67f,   2.33f,   3.33f,  4.67f,  7.00f, 10.33f, 15.00f
};

void ULidarScanner::Initialize(const FLidarConfig& InConfig, ELidarPreset InPreset)
{
	Config = InConfig;
	QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(LidarScan), false);
	QueryParams.bReturnPhysicalMaterial = false;

	BuildElevationAngles(InPreset);

	ChannelBuffer.SetNum(ElevationAngles.Num());
	for (FChannelHits& Ch : ChannelBuffer)
	{
		Ch.Points.Reserve(Config.PointsPerChannel);
		Ch.Ranges.Reserve(Config.PointsPerChannel);
	}

	LastScan.Points.Reserve(Config.TotalPoints());
	LastScan.Intensities.Reserve(Config.TotalPoints());
}

void ULidarScanner::UpdateConfig(const FLidarConfig& InConfig, ELidarPreset InPreset)
{
	Config = InConfig;
	BuildElevationAngles(InPreset);

	ChannelBuffer.SetNum(ElevationAngles.Num());
	for (FChannelHits& Ch : ChannelBuffer)
	{
		Ch.Points.Reset();
		Ch.Ranges.Reset();
	}
}

void ULidarScanner::BuildElevationAngles(ELidarPreset InPreset)
{
	switch (InPreset)
	{
	case ELidarPreset::VelodyneVLP16:
		ElevationAngles = ElevTable_VLP16;
		break;

	case ELidarPreset::VelodyneVLP32:
		ElevationAngles = ElevTable_VLP32C;
		break;

	case ELidarPreset::OusterOS1_64:
	case ELidarPreset::Livox_Mid360:
	case ELidarPreset::Custom:
	default:
		ElevationAngles.SetNum(Config.NumChannels);
		const float Step = (Config.NumChannels > 1)
			? (Config.VerticalFOVUpper - Config.VerticalFOVLower) / (Config.NumChannels - 1)
			: 0.f;
		for (int32 i = 0; i < Config.NumChannels; ++i)
			ElevationAngles[i] = Config.VerticalFOVLower + Step * i;
		break;
	}
}

void ULidarScanner::PerformScan(UWorld* World, const FTransform& SensorOrigin, AActor* OwnerToIgnore)
{
	if (!World) return;

	QueryParams.ClearIgnoredActors();
	if (OwnerToIgnore)
		QueryParams.AddIgnoredActor(OwnerToIgnore);

	const FVector SensorLoc  = SensorOrigin.GetLocation();
	const FQuat   SensorQuat = SensorOrigin.GetRotation();
	const float   MaxRange   = Config.MaxRange;
	const float   MinRange   = Config.MinRange;
	const float   NoiseStd   = Config.RangeNoiseStdDev;
	const int32   NumPts     = Config.PointsPerChannel;
	const float   HorizFOV   = Config.HorizontalFOV;

	for (int32 Ch = 0; Ch < ElevationAngles.Num(); ++Ch)
	{
		const float ElevRad = FMath::DegreesToRadians(ElevationAngles[Ch]);
		const float CosElev = FMath::Cos(ElevRad);
		const float SinElev = FMath::Sin(ElevRad);

		FChannelHits& Hits = ChannelBuffer[Ch];
		Hits.Points.Reset();
		Hits.Ranges.Reset();

		FHitResult Hit;
		for (int32 Pt = 0; Pt < NumPts; ++Pt)
		{
			const float AzimRad = FMath::DegreesToRadians((float(Pt) / NumPts) * HorizFOV);
			const FVector LocalDir(
				CosElev * FMath::Cos(AzimRad),
				CosElev * FMath::Sin(AzimRad),
				SinElev);

			const FVector WorldDir = SensorQuat.RotateVector(LocalDir);
			const FVector End      = SensorLoc + WorldDir * MaxRange;

			if (!World->LineTraceSingleByChannel(Hit, SensorLoc, End, ECC_Visibility, QueryParams))
				continue;
			if (!Hit.bBlockingHit || Hit.Distance < MinRange)
				continue;

			FVector HitPoint = Hit.ImpactPoint;
			if (NoiseStd > 0.f)
				HitPoint += WorldDir * FMath::FRandRange(-NoiseStd, NoiseStd);

			Hits.Points.Add(HitPoint);
			Hits.Ranges.Add(Hit.Distance);
		}
	}

	FlattenChannels();
}

void ULidarScanner::FlattenChannels()
{
	LastScan.Reset();

	for (const FChannelHits& Ch : ChannelBuffer)
	{
		for (int32 i = 0; i < Ch.Points.Num(); ++i)
		{
			LastScan.Points.Add(Ch.Points[i]);
			LastScan.Intensities.Add(
				FMath::Clamp(1.f - (Ch.Ranges[i] / Config.MaxRange), 0.f, 1.f));
		}
	}

	LastScan.PointCount = LastScan.Points.Num();
	LastScan.Timestamp  = FSensorTimestamp::Now();
}
