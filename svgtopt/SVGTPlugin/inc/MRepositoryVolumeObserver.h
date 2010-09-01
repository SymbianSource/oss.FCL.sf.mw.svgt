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
* Description:  Definition of the interface MRepositoryVolumeObserver used to
*                observer events from CRepositoryVolumeListener
*
*/


//
// JRT 22_09_04
// Version 1.0 release.

#ifndef REPOSITORYVOLUMEOBSERVER_H
#define REPOSITORYVOLUMEOBSERVER_H

class MRepositoryVolumeObserver
{
    public:
        virtual void VolumeChanged(TInt aNewValue) = 0;
};

#endif  //REPOSITORYVOLUMEOBSERVER_H

