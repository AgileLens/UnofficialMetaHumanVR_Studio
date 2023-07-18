/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#pragma once
#include "ILiveLinkSource.h"

#include "IOculusXRMovementModule.h"
#include "OculusXRMovement.h"
#include "OculusXRMovementLiveLink.h"

#define LOCTEXT_NAMESPACE "OculusXRMovement"

//-------------------------------------------------------------------------------------------------
// FOculusXRMovementModule
//-------------------------------------------------------------------------------------------------

class FOculusXRMovementModule : public IOculusXRMovementModule
{
public:
	FOculusXRMovementModule();

	static inline FOculusXRMovementModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOculusXRMovementModule>("OculusXRMovement");
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/* Live link */
	virtual TSharedPtr<ILiveLinkSource> GetLiveLinkSource() override;
	virtual bool IsLiveLinkSourceValid() const override;
	virtual void AddLiveLinkSource() override;
	virtual void RemoveLiveLinkSource() override;

private:
	TSharedPtr<MetaXRMovement::LiveLinkSource> MovementSource{ nullptr };
};

#undef LOCTEXT_NAMESPACE
