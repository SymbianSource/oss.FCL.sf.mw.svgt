/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVGT Plugin Implementation source file
 *
*/


#include "Svgtplugin.h"


#include <implementationproxy.h>
#include <cecombrowserplugininterface.h>
/***************************************************/
/**********ECOM STYLE SVGT PLUGIN ENTRY POINTS******/
/***************************************************/
CSvgtPluginEcomMain* CSvgtPluginEcomMain::NewL(TAny* aInitParam)
{
    TFuncs* funcs = REINTERPRET_CAST( TFuncs*, aInitParam);
    CSvgtPluginEcomMain* self = new(ELeave)CSvgtPluginEcomMain(funcs->iNetscapeFuncs);
    CleanupStack::PushL(self);
    self->ConstructL(funcs->iPluginFuncs);
    CleanupStack::Pop();

	Dll :: SetTls ( (void*) self );

    return self;
}

void CSvgtPluginEcomMain::ConstructL(NPPluginFuncs* aPluginFuncs)
{
	InitializeFuncs(aPluginFuncs);
}

CSvgtPluginEcomMain::CSvgtPluginEcomMain(NPNetscapeFuncs* aNpf) : CEcomBrowserPluginInterface(),iNpf(aNpf)
{
}

CSvgtPluginEcomMain::~CSvgtPluginEcomMain()
{

}
const TImplementationProxy KImplementationTable[] =
    {
        {{KSvgtPluginImplementationValue}, (TProxyNewLPtr) CSvgtPluginEcomMain::NewL}
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns the filters implemented in this DLL
// Returns: The filters implemented in this DLL
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
}



/**
  * The E32Dll() entry point function.
**/


EXPORT_C NPError InitializeFuncs(NPPluginFuncs* aPpf)
{
    aPpf->size					= sizeof(NPPluginFuncs);
	aPpf->version				= 1;
    aPpf->newp          = NewNPP_NewProc(SvgtPluginNewp);
    aPpf->destroy       = NewNPP_DestroyProc(SvgtPluginDestroy);
    aPpf->setwindow     = NewNPP_SetWindowProc(SvgtPluginSetWindow);
    aPpf->newstream     = NewNPP_NewStreamProc(SvgtPluginNewStream);
    aPpf->destroystream = NewNPP_DestroyStreamProc(SvgtPluginDestroyStream);
    aPpf->asfile        = NewNPP_StreamAsFileProc(SvgtPluginAsFile);
    aPpf->writeready    = NewNPP_WriteReadyProc(SvgtPluginWriteReady);
    aPpf->write         = NewNPP_WriteProc(SvgtPluginWrite);
    aPpf->print         = NewNPP_PrintProc(SvgtPluginPrint);
    aPpf->event         = NewNPP_HandleEventProc(SvgtPluginEvent);
    aPpf->urlnotify     = NewNPP_URLNotifyProc(SvgtPluginURLNotify);
    aPpf->javaClass     = NULL;
    aPpf->getvalue				= NewNPP_GetValueProc(SvgtPluginGetValue);
    aPpf->setvalue				= NewNPP_SetValueProc(SvgtPluginSetValue);

    return NPERR_NO_ERROR;
}

EXPORT_C void NPP_Shutdown(void)
{
	CSvgtPluginEcomMain* npm = (CSvgtPluginEcomMain*) Dll :: Tls ();
    delete npm;
	Dll :: SetTls ( NULL );
}

