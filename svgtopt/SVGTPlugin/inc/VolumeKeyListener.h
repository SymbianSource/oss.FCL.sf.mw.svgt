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


#ifndef VOLUMEKEYLISTENER_H
#define VOLUMEKEYLISTENER_H

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>

#include <centralrepository.h>
#include<remconcoreapitargetobserver.h>    //for MRemConCoreApiTargetObserver
#include<remconinterfaceselector.h>     //for CRemConInterfaceSelector
#include<remconcoreapitarget.h>         //for CRemConCoreApiTarget

//
// for emulator max volume = 65535
#define WINS_DEFAULT_VOLUME 43690

//
// For HW max volume = 9
#define HW_DEFAULT_VOLUME 6

class MRepositoryVolumeObserver;

// DATA TYPES
// Used for volume up/down events
enum TVolumeControlCommands
    {
    EVolumeControlCmdVolumeUpByOne,
    EVolumeControlCmdVolumeDownByOne
    };

NONSHARABLE_CLASS( CVolumeKeyListener ) : public CBase, 
                                  public MRemConCoreApiTargetObserver
    {
public:
    
    static CVolumeKeyListener* NewL( MRepositoryVolumeObserver* aObserver );
    virtual ~CVolumeKeyListener();
    TInt GetCurrentVolume();
    /**
     * Function to add an observer
     */
    void SetObserver( MRepositoryVolumeObserver* aObserver ); 
    
private:
    CVolumeKeyListener( MRepositoryVolumeObserver* aObserver );
    void ConstructL();

    //From MRemConCoreApiTargetObserver (remconcoreapitargetobserver.h)
    void MrccatoCommand( TRemConCoreApiOperationId aOperationId, 
                         TRemConCoreApiButtonAction aButtonAct );
    
    /**
      * Handle repeat event
      */
    void HandleRepeatEvent();

    /**
    * Send play command to app UI or view.
    * @since 3.1
    */
    void FilterAndSendCommand(TInt aCommandId);
   /**
    * Callback for timer
    */
    static TInt TimerCallback(TAny* aPtr);

private:
    CRemConCoreApiTarget* iInterfaceSelector;
    CRemConInterfaceSelector* iSelector;
    CRepository* iCenRepos;
    CPeriodic* iTimer;                                   // owned
    TInt iCommandId;
    // Stores the observers.
    MRepositoryVolumeObserver* iObserver;
    };

#endif  //VOLUMEKEYLISTENER_H

// End of File
