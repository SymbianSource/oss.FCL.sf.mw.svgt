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
* Description:  SVGT Plugin Implementation header file
 *
*/


#ifndef __SVGTPLUGIN_H
#define __SVGTPLUGIN_H

#include <e32std.h>
#include <coecntrl.h>
#include <coemain.h>
#include <fbs.h>
#include <barsread.h>

#include <hwrmlight.h>
//SVGT Engine interface
#include <SVGRequestObserver.h>
#include <SVGEngineInterfaceImpl.h>
#include <SVGHyperlinkListener.h>
#include "SVGListener.h"

#include <npupp.h>
#include <pluginadapterinterface.h>

#include <cecombrowserplugininterface.h>

#include "MRepositoryVolumeObserver.h"

class   CSvgtPlugin;
class   CSvgEngineInterfaceImpl;
class   CFbsBitmap;

class   CVolumeKeyListener;
class   CRepositoryVolumeListener;
/******************************************
This class is specific to ECom Style Plugin.
This is used for passing plugin specific initialization information to and from
browser.
*******************************************/

IMPORT_C NPError InitializeFuncs(NPPluginFuncs* aPpf);

const TInt KSvgtPluginImplementationValue = 0x101F8823; //implementation id Svgt Ecom Plugin

class CSvgtPluginEcomMain :  public CEcomBrowserPluginInterface
	{
    public: // Functions from base classes
        static CSvgtPluginEcomMain* NewL(TAny* aInitParam);
        ~CSvgtPluginEcomMain();
        NPNetscapeFuncs* Funcs() {return iNpf;}
    private:
        CSvgtPluginEcomMain(NPNetscapeFuncs* aNpf);
        void ConstructL(NPPluginFuncs* aPluginFuncs);
        NPNetscapeFuncs* iNpf;
	};

/****************************************
Class CSvgtPluginControl
Class to Handle UI related actions
****************************************/

class CSvgtPluginControl : public CCoeControl, public MCoeControlObserver,
                           public MPluginNotifier, public MHWRMLightObserver

    {
public:
    ~CSvgtPluginControl();
    void ConstructL(CSvgtPlugin *aPlugin, const CCoeControl *aParentControl);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void FocusChanged(TDrawNow aDrawNow);
private: // new - possible notifications
#ifndef ER5
    TCoeInputCapabilities InputCapabilities() const;
#endif
    void ApplicationFocusChanged(TBool aInFocus);
    void SystemNotification();
    void Draw(const TRect& aRect) const;
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    void SizeChanged();
    void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);
private: // in MPluginNottify in S60
    TInt NotifyL(TNotificationType aCallType, void* aParam);
public: //From MEikMenuObserver


public:
    void LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus);
	CHWRMLight* iLightObserver; 
    void AsFile(const TDesC& fname,NPStream* stream);
    TBool IsAsFileCalled();
    TDesC& GetFilename();

private:
    void UpdatePointer(TInt aShiftX =0 , TInt aShiftY=0);

public:
	TSize iContentDimension;
private:
    CSvgtPlugin *iPlugin;
    TInt iPointerDx, iPointerDy;// increment decrement
    TInt iPointerX, iPointerY;//actual Pointer position with respect to Document.
    TBool iAsFileCalled;
    TBuf<256> iFilename;
    TBool iShowCursor;
	TInt iXPosAtMouseDown;
    TInt iYPosAtMouseDown;
	TKeyEvent iPreviousScanCode; //Introduced to differentiate long keypress and short key press

    };


/*******************************************************
Class CSvgtPlugin
********************************************************/

class CSvgtPlugin : public CBase,
                    public MSvgRequestObserver,
                    public MSvgHyperlinkListener,
                    public MSvgViewPortListener
                    ,
                    public MRepositoryVolumeObserver
    {
	struct ImageInfo
		{
		TBuf<256>						iImageRefUri;
		TBool							iImagePendingOps;
		};
		
public:
    static CSvgtPlugin* NewL(NPP anInstance);
    ~CSvgtPlugin();
    void ConstructL(NPP anInstance);
    void PrepareToExitL();
    CSvgtPluginControl* Control();
    void GetSizeOfRenderBuffer(TSize& aSize);
    NPP Instance();
    void SetWindowL(const NPWindow  *aWindow,const TRect& aRect);
    /**
     * ResizeWindow This function set width & height to the browser
     * and tell brower to resize.
     *
     * @since S60 3.1
     * @param TInt width that needed to be used.
     * @param TInt height that needed to be used.
     * @return void
     */
    void ResizeWindow(TInt width, TInt height);

    /**
     * AdjustDimention
     *
     * @since S60 3.1
     * @return ETrue if dimention needs to be changed. EFalse if dimention
     * remains the same.
     */
     TBool AdjustDimention( TBool aWidthInPercentage, 
             TBool aHeightInPercentage );

    /**
     * This function retrieves attributes from HTML that is
     * passed in by browser engine.
     *
     * @since S60 3.1
     * @param NPMIMEType
     * @param NPP
     * @param uint16
     * @param DesC16Array* attribute names
     * @param CDesC16Array* attribute values
     * @param NPSavedData*
     * @return NPError
     */
     NPError SvgPluginNew(NPMIMEType pluginType, NPP instance, uint16 mode,
        CDesC16Array* argn, CDesC16Array* argv, NPSavedData* saved);

public: // implements MSvgRequestObserver

        void    UpdateScreen();

        TBool   ScriptCall( const TDesC& aScript,
                                    CSvgElementImpl* aCallerElement );

        TInt FetchImage( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );
		
		TInt FetchFont( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );
		
        void    GetSmilFitValue(TDes& aSmilValue);

        void    UpdatePresentation(const TInt32&  aNoOfAnimation);
        // MHyperlinkListener interface methods
        TBool   LinkEntered( const TDesC& aUri );
        TBool   LinkExited( const TDesC& aUri );
        TBool   LinkActivated( const TDesC& aUri );
        TBool   LinkActivatedWithShow(const TDesC& aUri,const TDesC& aShow);
        //MSvgViewPortListener method
        void GetViewPort(TInt getWidth, TInt getHeight,TBool isWidthInPer, TBool isHeightInPer, TInt &setWidth, TInt &setHeight);

        //  From MRepositoryVolumeObserver
        void VolumeChanged(TInt aNewValue);
        
		TBool   LoadSvgFile( const TDesC& aFilename );
		
private:

        void FetchImageL(const TDesC&  aUri);       
public:
    CFbsBitmap *                iRenderBuffer;
	TSize						iControlSize;
 	TBool          				iSetWindowCalled;
 	TBool                       iStartCalled;
    CSvgEngineInterfaceImpl *    iSvgModule;
    TInt iRenderWidth;
    TInt iRenderHeight;
    TInt iObjectWidth;
    TInt iObjectHeight;
    NPWindow iCurrentWindow;

    TBool   iTotalNumerOfAnimation, iStaticContent;
	
	/*Image Rendering*/
	RPointerArray<HBufC>	        iImageUrlInfoList;

    TInt                iViewPortWidth;
    TInt                iViewPortHeight;
private:

    CVolumeKeyListener* iKeyListener;
    CRepositoryVolumeListener* iRepositoryListener;
    
    MPluginAdapter*             iPluginAdapter;

    //Control of UI for SVGT Engine
    CCoeControl             *   iParentControl;
    CSvgtPluginControl      *   iControl;
    NPP                         iInstance;
    TBool                       iIsRenderBufferDefined;
    TInt                        iResourceFileIndex;
    TBool						iExpectedDefaultSize;
    TBool						iAdjusted;
    // Flag indicates whether object height is specified 
    // as percentage value
    TBool                       iObjHeightInPercentage;
    // Flag indicates whether object width is specified 
    // as percentage value    
    TBool                       iObjWidthInPercentage;
    
    //While parsing are object width height are specified or not
    TBool   iObjectWidthSpecified;
    TBool   iObjectHeightSpecified;
    };


/*********************************************************************************

    Plugin API methods

*/

/**
  * Create a plugin.
  */
NPError SvgtPluginNewp(NPMIMEType pluginType, NPP instance, uint16 mode, CDesCArray* argn, CDesCArray* argv, NPSavedData* saved);

/**
  * Destroy a plugin.
  */
NPError SvgtPluginDestroy(NPP instance, NPSavedData** save);

/**
  * This is the parent window of a plugin.
  */
NPError SvgtPluginSetWindow(NPP instance, NPWindow* window);

/**
  * A new data stream has been created for sending data to the plugin.
  */
NPError SvgtPluginNewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);

/**
  * A data stream has been terminated.
  */
NPError SvgtPluginDestroyStream(NPP instance, NPStream* stream, NPReason reason);

/**
  * A data stream has been fully saved to a file.
  */
void SvgtPluginAsFile(NPP instance, NPStream* stream, const TDesC& fname);

/**
  * Return the maximum number of bytes this plugin can accept from the stream.
  */
int32 SvgtPluginWriteReady(NPP instance, NPStream* stream);

/**
  * Receive more data - return number of bytes processed.
  */
int32 SvgtPluginWrite(NPP instance, NPStream* stream, int32 offset, int32 len, void* buffer);

/**
  * Generic hook to set values/attributes within the plugin.
  */
NPError SvgtPluginSetValue(NPP instance, NPNVariable variable, void *ret_value);

/**
  * Generic hook to get values/attributes from the plugin.
  */
NPError SvgtPluginGetValue(NPP instance, NPNVariable variable, void* *ret_value);

/**
  * Event
  */
int16 SvgtPluginEvent(NPP instance, void* event);


void SvgtPluginURLNotify(NPP instance, const TDesC& url, NPReason reason, void* notifyData);

void SvgtPluginPrint(NPP instance, NPPrint* platformPrint);

#endif // __PLGPLUGIN_H