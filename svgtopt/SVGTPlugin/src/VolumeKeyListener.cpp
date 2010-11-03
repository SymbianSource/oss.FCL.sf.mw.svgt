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
* Description:  It completes on a change in the volume property.  Used for 
*                the new Central Repository Server.  Used in EKA2 builds only
*
*/


#include <eikapp.h>
#include <aknconsts.h>
#include <browseruisdkcrkeys.h>

#include "VolumeKeyListener.h" 
#include "MRepositoryVolumeObserver.h"

// CONSTANTS
const TInt KFirstTimerExpiryInterval = 1; // Expire immediately
const TInt KTimerExpiryInterval = KAknStandardKeyboardRepeatRate;

const TInt KMinVolume = 0;  // Minimum volume level(Mute)
const TInt KMaxVolume = 10; // Maximum volume level

// ----------------------------------------------------
// CVolumeKeyListener::NewL
// Description: static constructor.
// Output: none
// Return: CVolumeKeyListener object
// ----------------------------------------------------
CVolumeKeyListener* CVolumeKeyListener::NewL( MRepositoryVolumeObserver* aObserver )
    {
    CVolumeKeyListener* self = new(ELeave) CVolumeKeyListener(
                                                                aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------
// CVolumeKeyListener::CVolumeKeyListener
// Description: constructor.
// Output: none
// Return: none
// ----------------------------------------------------
CVolumeKeyListener::CVolumeKeyListener( MRepositoryVolumeObserver* aObserver ):
                                                iInterfaceSelector(NULL),
                                                iSelector(NULL),
                                                iCenRepos(NULL),
                                                iObserver(aObserver)
    {
    }   

// ----------------------------------------------------
// CVolumeKeyListener::ConstructL
// Description: second phase constructor.
// Input:  aObserver: link to the calling object
// Output: none
// Return: none
// ----------------------------------------------------
void CVolumeKeyListener::ConstructL()
    {
    //
    // Open a connection to receive Volume Key events.
    iSelector = CRemConInterfaceSelector::NewL();
    iInterfaceSelector = CRemConCoreApiTarget::NewL(*iSelector,*this);
    TRAP_IGNORE(iSelector->OpenTargetL());
    // Timer for implementing repeat
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);

    //
    // Open a connection to the Central Repository...
    iCenRepos = CRepository::NewL(KCRUidBrowser);
    //    iObservers=new(ELeave)CArrayFixSeg<MVolumePropertyWatchObserver*>
    //                (KVolumeObserversArrayGranularity);
    }

// ----------------------------------------------------
// CVolumeKeyListener::~CVolumeKeyListener
// Description: destructor.
// Input:  none
// Output: none
// ----------------------------------------------------
CVolumeKeyListener::~CVolumeKeyListener()
    {
    if(iSelector)
        {
        delete iSelector;
        iInterfaceSelector=NULL;
        iSelector=NULL; //iSelector has been deleted by "delete iInterfaceSelector"
        }

    if(iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        }

    if(iCenRepos)
        {
        delete iCenRepos;
        }
    }

// ----------------------------------------------------
// CVolumeKeyListener::SetObserver
// Description: Used to set an observer 
// Input:  aObserver: Point to observer
// Output: none
// Return: none
// ----------------------------------------------------
void CVolumeKeyListener::SetObserver( MRepositoryVolumeObserver* aObserver)
    {
    iObserver = aObserver;
    }

// ----------------------------------------------------
// CVolumeKeyListener::GetCurrentVolume
// Description: gets the current volume level from the 
//   central directory.
// Input:  none
// Output: none
// Return: volume level
// ----------------------------------------------------
TInt CVolumeKeyListener::GetCurrentVolume()
    {
    TInt currentVolume = 0;
    //
    // Get the current volume
    if(iCenRepos)
        iCenRepos->Get(KBrowserMediaVolumeControl, currentVolume);

/*
    #if defined(__WINSCW__) //offset required on the emulator
        currentVolume += WINS_DEFAULT_VOLUME;
    #endif
*/
    return currentVolume;
    }

// ----------------------------------------------------
// CVolumeKeyListener::MrccatoCommand
// Description: A Volume key command has been received
//        (from MRemConCoreApiTargetObserver)
// Input:aOperationId The operation ID of the command
//         aButtonAct The button action associated with the command.
// Output: none
// Return: none
// ----------------------------------------------------
void CVolumeKeyListener::MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
                             TRemConCoreApiButtonAction aButtonAct )
    {

    switch(aOperationId)
        {
        case ERemConCoreApiVolumeUp:
            {
            switch (aButtonAct)
                {
                case ERemConCoreApiButtonPress:
                    {
                     //Start Timer
                     if (!iTimer->IsActive())
                        {
                        iCommandId = EVolumeControlCmdVolumeUpByOne;
                        iTimer->Start(KFirstTimerExpiryInterval,
                                      KTimerExpiryInterval, 
                                      TCallBack(TimerCallback, this));
                        }
                    break;
                    }
                case ERemConCoreApiButtonRelease:
                    {
                    iTimer->Cancel();
                    break;
                    }
                case ERemConCoreApiButtonClick:
                    {
                    FilterAndSendCommand(EVolumeControlCmdVolumeUpByOne);
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }
        case ERemConCoreApiVolumeDown:
            {
            switch (aButtonAct)
                {
                case ERemConCoreApiButtonPress:
                    {
                    //Start Timer
                    iCommandId = EVolumeControlCmdVolumeDownByOne;
                    if (!iTimer->IsActive())
                        {
                        iTimer->Start(KFirstTimerExpiryInterval,
                                      KTimerExpiryInterval, 
                                      TCallBack(TimerCallback, this));
                        }
                     break;
                    }
                case ERemConCoreApiButtonRelease:
                    {
                    iTimer->Cancel();
                    break;
                    }
                case ERemConCoreApiButtonClick:
                    {
                    FilterAndSendCommand(EVolumeControlCmdVolumeDownByOne);
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }        
    }        
        
// -----------------------------------------------------------------------
// CVolumeKeyListener::HandleRepeatEvent
// -----------------------------------------------------------------------
//
void CVolumeKeyListener::HandleRepeatEvent()
    {
    FilterAndSendCommand(iCommandId);
    }

// -----------------------------------------------------------------------
// CVolumeKeyListener::TimerCallback
// -----------------------------------------------------------------------
//
TInt CVolumeKeyListener::TimerCallback(TAny* aPtr)
    {
    static_cast<CVolumeKeyListener*>(aPtr)->HandleRepeatEvent();
    return KErrNone;
    }

// -----------------------------------------------------------------------
// CVolumeKeyListener::FilterAndSendCommand
// -----------------------------------------------------------------------
//
void CVolumeKeyListener::FilterAndSendCommand(TInt aCommandId)
    {
    TInt currentVolume = 0;

    iCenRepos->Get(KBrowserMediaVolumeControl,currentVolume);
    switch(aCommandId)
        {
        case EVolumeControlCmdVolumeUpByOne:
            currentVolume=(currentVolume< KMaxVolume)? ++currentVolume : KMaxVolume;
            break;
        case EVolumeControlCmdVolumeDownByOne:
            currentVolume=(currentVolume> KMinVolume)? --currentVolume : KMinVolume;
            break;  
        default:
            break;      
        }
         
    iCenRepos->Set(KBrowserMediaVolumeControl,currentVolume);
    
#if defined(__WINS__) //offset required on the emulator
        currentVolume += WINS_DEFAULT_VOLUME;
#endif   
    
    if(iObserver)
        iObserver->VolumeChanged(currentVolume);
    }

// End of File
