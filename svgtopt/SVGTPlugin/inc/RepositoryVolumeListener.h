/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  AO that completes on a change in the volume property.  Used for 
*                the new Central Repository Server.  Used in EKA2 builds only
*
*/


//
#ifndef REPOSITORYVOLUMELISTENER_H
#define REPOSITORYVOLUMELISTENER_H

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>

//
// for emulator max volume = 65535
#define WINS_DEFAULT_VOLUME 43690

//
// For HW max volume = 9
#define HW_DEFAULT_VOLUME 6

#include <centralrepository.h>

class MRepositoryVolumeObserver;

NONSHARABLE_CLASS( CRepositoryVolumeListener ) : public CActive
	{
public:
	enum TPropertyWatchState
    	{
    	EConnected = 1,
    	EUnableToConnect
    	};
	
	static CRepositoryVolumeListener* NewL(MRepositoryVolumeObserver* aObserver);
    virtual ~CRepositoryVolumeListener();
    TInt GetCurrentVolume();
                                   
private:
	CRepositoryVolumeListener();
	void ConstructL(MRepositoryVolumeObserver* aObserver);
	TInt GetDefaultVolume();

    //From CActive	
    void RunL();	
    void DoCancel();	
	

	CRepository* iCenRepos;

    TPropertyWatchState iState;
	
    MRepositoryVolumeObserver* iObserver;
	};

#endif  //REPOSITORYVOLUMELISTENER_H
