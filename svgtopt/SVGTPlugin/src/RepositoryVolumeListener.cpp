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


//#include <SharedDataClient.h>


#include "RepositoryVolumeListener.h"
//#include <SharedDataKeys.h> //include this file when KSDUidMediaVolumeControl and KMediaVolume will be needed
//#include "epocdebug.h"
#include <BrowserUiSDKCRKeys.h>
#include "MRepositoryVolumeObserver.h"

CRepositoryVolumeListener* CRepositoryVolumeListener::NewL(MRepositoryVolumeObserver* aObserver)
    {
    CRepositoryVolumeListener* self = new(ELeave) CRepositoryVolumeListener;
    CleanupStack::PushL(self);
    self->ConstructL(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

CRepositoryVolumeListener::CRepositoryVolumeListener() : CActive(CActive::EPriorityStandard)
    {
    iState = EUnableToConnect;
    CActiveScheduler::Add(this) ; // add to active scheduler
    }

void CRepositoryVolumeListener::ConstructL(MRepositoryVolumeObserver* aObserver)
    {
    //
    // Open a connection to the Central Repository...
    iCenRepos = CRepository::NewL(KCRUidBrowser);

    if ( iCenRepos->NotifyRequest(KBrowserMediaVolumeControl, iStatus) == KErrNone )
        {
        iState = EConnected;	
        SetActive();
        }
    else
        {
        iState = EUnableToConnect;
        }

    iObserver = aObserver;
    }

CRepositoryVolumeListener::~CRepositoryVolumeListener()
    {
	Cancel();
	delete iCenRepos;  // close the client session
    }

//
// Get the current volume
TInt CRepositoryVolumeListener::GetCurrentVolume()
    {
    TInt currentVolume = 0;

    if ( iState == EUnableToConnect )
        {
    	//
    	// Connection to CenRepos not available so return the default volume
    	currentVolume = GetDefaultVolume();
        }
    else
        {
    	//
    	// Get the current volume
    	if ( iCenRepos->Get(KBrowserMediaVolumeControl, currentVolume) != KErrNone )
            {
            // Error retrieving value... return default value
            //PLAYEROUTPUT("*ERROR* CRepositoryVolumeListener::GetCurrentVolume() - Unable to retreive volume value");
            currentVolume = GetDefaultVolume();
            }
        }
    
    return currentVolume;
    }

//
// Return the default volume in this case 2/3 of the max support by the system
TInt CRepositoryVolumeListener::GetDefaultVolume()
    {
    TInt defaultVol = 0;

    #if defined(__WINS__)
        defaultVol = WINS_DEFAULT_VOLUME;
    #else
        defaultVol = HW_DEFAULT_VOLUME;
    #endif

    return defaultVol;
    }

void CRepositoryVolumeListener::DoCancel()
    {
    iCenRepos->NotifyCancel(KBrowserMediaVolumeControl);
    }

//
// Activated when the watched value updates.
void CRepositoryVolumeListener::RunL()
    {
    TInt currentVolume = 0;

    if (EUnableToConnect == iState)
        {
        return;
        }

    //
    // Get the current volume
    if ( iCenRepos->Get(KBrowserMediaVolumeControl, currentVolume) == KErrNone )
        {
        //
        // Inform the observer
        iObserver->VolumeChanged(currentVolume);
        if ( iCenRepos->NotifyRequest(KBrowserMediaVolumeControl, iStatus) == KErrNone )
            {
            SetActive();
            }
        else
            {
            iState = EUnableToConnect;
            }
        }
    else
        {
        //
        // error
        iState = EUnableToConnect;		
        }

    }
