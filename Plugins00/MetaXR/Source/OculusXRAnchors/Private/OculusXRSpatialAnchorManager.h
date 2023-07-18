/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.

This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#pragma once

#include "CoreMinimal.h"
#include "OculusXRAnchorManager.h"

namespace OculusXRAnchors
{
	struct FOculusXRSpatialAnchorManager : FOculusXRAnchorManager
	{
		FOculusXRSpatialAnchorManager()
			: FOculusXRAnchorManager()
		{
		}

		static bool CreateSpatialAnchor(const FTransform& InTransform, uint64& OutRequestId);
	};
} // namespace OculusXRAnchors
