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


/*
************************************************************************************
* Contents:           Example plugin implementation
*                     Implementation of the API used by Opera to control the plugin
************************************************************************************
*/


#include "Svgtplugin.h"

/**
  * Create a new instance of a SVG plugin.
**/

NPError SvgtPluginNewp(NPMIMEType pluginType, NPP instance, uint16 mode,
    CDesCArray* argn, CDesCArray* argv, NPSavedData* saved)
	{
	CSvgtPlugin* lSvgtPlugin = NULL;
    TRAPD(err,lSvgtPlugin = CSvgtPlugin::NewL(instance));

	if (err == KErrNoMemory)
        return NPERR_OUT_OF_MEMORY_ERROR;
    if (err != KErrNone)
        return NPERR_MODULE_LOAD_FAILED_ERROR;

    instance->pdata = (void *) lSvgtPlugin;
    NPError nerr = lSvgtPlugin->SvgPluginNew(pluginType, instance,
        mode, argn, argv, saved);
    return nerr;
	}

/**
  * Destroy a plugin.
**/
NPError SvgtPluginDestroy(NPP instance, NPSavedData** /*save*/)
	{
	CSvgtPlugin* lSvgtPlugin = (CSvgtPlugin *)instance->pdata;
	if ( lSvgtPlugin )
		{
		TRAPD(err,lSvgtPlugin->PrepareToExitL());
        if (err != KErrNone)
            {
            #ifdef _DEBUG
            RDebug::Printf("SvgtPluginDestroy Error when printing Warning.");
            #endif //_DEBUG
            }
		/**Ignoring the error*/
		delete lSvgtPlugin;
		lSvgtPlugin = NULL;
		}
	return NPERR_NO_ERROR;
	}

/**
  * This is the parent window of a plugin.
  */
NPError SvgtPluginSetWindow(NPP instance, NPWindow *window)
	{
    CSvgtPlugin* lSvgtPlugin = (CSvgtPlugin *) instance->pdata;
	TUint lWidth = window->width;
	TUint lHeight = window->height;
	
	TRAPD(err,lSvgtPlugin->SetWindowL(window,TRect(TSize(lWidth,lHeight))));

    if (err == KErrNoMemory)
        return NPERR_OUT_OF_MEMORY_ERROR;
    if (err != KErrNone)
        return NPERR_GENERIC_ERROR;
    return NPERR_NO_ERROR;
	}

/**
  * A new data stream has been created for sending data to the plugin.
  * @param stream - A pointer to plugin specific data can be placed in stream->pdata
  *
  */

NPError SvgtPluginNewStream(NPP /*instance*/, NPMIMEType /*type*/, NPStream* /*stream*/, NPBool /*seekable*/, uint16* stype)
	{
	*stype = NP_ASFILEONLY;
	return NPERR_NO_ERROR;
	}

/**
  * A data stream has been terminated.
  */
NPError SvgtPluginDestroyStream(NPP /*instance*/, NPStream* /*stream*/, NPReason /*reason*/)
	{
    return NPERR_NO_ERROR;
	}

/**
  * A data stream has been fully saved to a file.
  */
void SvgtPluginAsFile(NPP instance, NPStream* stream, const TDesC& fname)
	{
	CSvgtPlugin* lSvgtPlugin = (CSvgtPlugin*)instance->pdata;
	
	if (lSvgtPlugin && lSvgtPlugin->Control())
	  {	
		lSvgtPlugin->Control()->AsFile(fname, stream );
	  }
	}

/**
  * Return the maximum number of bytes this plugin can accept from the stream.
  */
int32 SvgtPluginWriteReady(NPP /*instance*/, NPStream* /*stream*/)
	{
	return 65536;
	}

/**
  * Receive more data
  * @param buffer - contains the data.
  * @param len    - the number of bytes in buffer.
  * @param offset - the number of bytes already sent/processed.
  * @return TInt number of bytes processed.
  */
int32 SvgtPluginWrite(NPP /*instance*/, NPStream* /*stream*/, int32 /*offset*/, int32 len, void* /*buffer*/)
	{
	return len;
	}

/**
  * Event
  */
int16 SvgtPluginEvent(NPP /*instance*/, void* /*event*/)
	{
    return 0;
	}


/**
  * Generic hook to set values/attributes within the plugin.
  */
NPError SvgtPluginSetValue(NPP /*instance*/, NPNVariable /*variable*/, void* /*ret_value*/)
	{
    return NPERR_NO_ERROR;
	}

/**
  * Generic hook to get values/attributes from the plugin.
  */
NPError SvgtPluginGetValue(NPP instance, NPNVariable variable, void*
*ret_value)
	{
    CSvgtPlugin* lSvgtPlugin = (CSvgtPlugin *)instance->pdata;
    if(lSvgtPlugin)
        {
        // A response of false when enum passed is NPPVpluginInteractiveBool
        // must be interpreted as "Plugin is interactive"
        // Since operations such as panning, zooming can be performed
        // on all svg contents plugin can be considered always interactive
        if(variable==NPPVpluginInteractiveBool)
    	    {
    	    *(TBool*) ret_value=EFalse;
    	    }
        }
    return NPERR_NO_ERROR;
	}
void SvgtPluginURLNotify(NPP /*instance*/, const TDesC& /*url*/, NPReason /*reason*/, void* /*notifyData*/)
	{
	}

void SvgtPluginPrint(NPP /*instance*/, NPPrint* /*platformPrint*/)
	{
	}
