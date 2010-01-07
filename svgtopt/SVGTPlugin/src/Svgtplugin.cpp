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



#include <e32std.h>
#include <eikenv.h>
#include <barsread.h>
#include <fbs.h>
#include <gdi.h>
#include <f32file.h>

#include <featmgr.h>

#include <eikappui.h>

#include <SVGEngineInterfaceImpl.h>

#include "Svgtplugin.h"

#include "VolumeKeyListener.h"
#include "RepositoryVolumeListener.h"

_LIT(KWidth, "width");
_LIT(KHeight, "height");

CSvgtPlugin* CSvgtPlugin::NewL(NPP anInstance)
	{
    CSvgtPlugin *self = new (ELeave) CSvgtPlugin;
	CleanupStack::PushL( self );
	self->ConstructL(anInstance);

	CleanupStack::Pop();
	return self;
	}


CSvgtPlugin::~CSvgtPlugin()
	{
	if ( iControl )
		{
    	delete iControl;
    	iControl=NULL;
    	}

	if ( iSvgModule )
		{
		iSvgModule->Stop();
		delete iSvgModule;
		iSvgModule = NULL;
		}

	if ( iRenderBuffer )
		{
		delete iRenderBuffer;
		iRenderBuffer = NULL;
		}
	
 	iImageUrlInfoList.ResetAndDestroy();        
	iImageUrlInfoList.Close();

	if(iKeyListener)
	    {
	    delete iKeyListener;
	    iKeyListener = NULL;
	    }

	if(iRepositoryListener)
	    {
	    delete iRepositoryListener;
	    iRepositoryListener = NULL;
	    }
	}

void CSvgtPlugin::PrepareToExitL()
	{
	if (iPluginAdapter)
		iPluginAdapter->PluginFinishedL();
	}

void CSvgtPlugin::ConstructL(NPP anInstance)
	{
  	iInstance=anInstance;
  	iRenderBuffer = new ( ELeave ) CFbsBitmap();
	
	iObjectWidth = 0;
	iObjectHeight = 0;
	// By default obj height is not in percentage
	iObjHeightInPercentage = EFalse; 
	// By default obj width is not in percentage
	iObjWidthInPercentage = EFalse; 
	iExpectedDefaultSize = EFalse;
	
	//While parsing are object width height are specified or not
	iObjectWidthSpecified = EFalse;
	iObjectHeightSpecified = EFalse;
	TFontSpec lFontSpec;
	iSvgModule = CSvgEngineInterfaceImpl::NewL( iRenderBuffer, this, lFontSpec );

  	// Clear background white
	iSvgModule->SetBackgroundColor(0xffffffff);
    iSvgModule->AddHyperlinkListener( this );
    iAdjusted = EFalse;

    if (FeatureManager::FeatureSupported(KFeatureIdSideVolumeKeys))
        {
        User::InfoPrint(_L("Volume Key supported"));
        iKeyListener = CVolumeKeyListener::NewL(this);
   	    }
   	else
   	    {
   	    User::InfoPrint(_L("Key not supported"));
   	    iRepositoryListener = CRepositoryVolumeListener::NewL(this);
   	    }
	}

void CSvgtPlugin::SetWindowL(const NPWindow *aWindow,const TRect& aRect)
    {
    iCurrentWindow.x = aWindow->x;
    iCurrentWindow.y = aWindow->y;
    iCurrentWindow.window = aWindow->window;
    iCurrentWindow.clipRect = aWindow->clipRect;
    iCurrentWindow.type = aWindow->type;
    iCurrentWindow.width = aWindow->width;
    iCurrentWindow.height = aWindow->height;

    iPluginAdapter = static_cast<MPluginAdapter*>(aWindow->window);
    iParentControl = iPluginAdapter->GetParentControl();
    if (iControl == NULL)
        {
        iControl = new (ELeave) CSvgtPluginControl;
        iControl->ConstructL(this,iParentControl);
        iPluginAdapter->PluginConstructedL(iControl);
        iPluginAdapter->SetPluginNotifier(iControl);
        }

	if ( !iControl || !iSvgModule || aRect.Width() <= 0 || aRect.Height() <= 0 )
    	{
		  return;
		  }
	iSetWindowCalled = ETrue;
	
	    iControl->SetRect(aRect);
	
	if ( iRenderWidth != aRect.Width() ||
	     iRenderHeight != aRect.Height() )
		{
		iRenderWidth = aRect.Width();
		iRenderHeight = aRect.Height();
		
		// Browser initially sends width/height as zero 
		// if width/height are not specified
		// Later if svg is available to us we call resize after adjustDimention
		// But if we dont do adjustDimention quick enough
		// Browser sends default width/Height as 300x150
		
		// To Ensure that we are setting the object width/height and 
		// browser is not setting default width/height ahead of us doing resize
		
		// In adjustDimention if width/height are specified at the browser level
		// iAdjusted is set to false, the second condition is to ensure that
		// if the browser does a resize at a later point it should be allowed
		if(iAdjusted || (iObjectWidthSpecified && iObjectHeightSpecified))
		    {
		    iObjectWidth  = iRenderWidth;
            iObjectHeight = iRenderHeight;
		    }
		iSvgModule->Pause(); //fix for CGSR-7CN7T7
		
		User::LeaveIfError( iRenderBuffer->Create(
							TSize( iRenderWidth, iRenderHeight ), EColor64K ) );
		iSvgModule->SetFrameBuffer( iRenderBuffer );
		iSvgModule->Resume(); // fix for CGSR-7CN7T7
		}


		if ( iExpectedDefaultSize )
			{
			iExpectedDefaultSize = EFalse;
            AdjustDimention(ETrue,ETrue);
			}

    	if(iControlSize != aRect.Size())
    		{
            iRenderWidth = aRect.Width();
            iRenderHeight = aRect.Height();
	    	CFbsBitmap * lTempRenderBuffer = new (ELeave) CFbsBitmap();
            CleanupStack::PushL(lTempRenderBuffer);
            User::LeaveIfError( lTempRenderBuffer->Create( TSize(iRenderWidth,iRenderHeight), EColor64K ));
			iSvgModule->SetFrameBuffer(lTempRenderBuffer);

			delete	iRenderBuffer;
			iRenderBuffer = lTempRenderBuffer;
			CleanupStack::Pop(lTempRenderBuffer);
			lTempRenderBuffer = NULL;
			iControlSize = aRect.Size();
			iControl->iContentDimension = iSvgModule->ContentDimensions();
			}
        // For autoloading feature.
            if ( iControl->IsAsFileCalled() && iControl->GetFilename().Length() > 0 &&
                 !iStartCalled )
            {

                    TBool widthInPercentage;
                    TBool heightInPercentage;
                    
	                iControl->iContentDimension = iSvgModule->ContentDimensionsInPercentage();
	                if(iControl->iContentDimension.iWidth == -1 )
	                {
	                    widthInPercentage = EFalse;
	                    if(iSvgModule->SvgDocument())
	                    {
	                        iControl->iContentDimension.iWidth = iSvgModule->GetViewportWidth(iSvgModule->SvgDocument());    
	                    }
	                    else
	                    {
	                        return;
	                    }
	                    
	                }
	                else 
                    {
	                    widthInPercentage = ETrue;
                    }
                    
                    if(iControl->iContentDimension.iHeight == -1)
                    {
                        heightInPercentage = EFalse;
                        if(iSvgModule->SvgDocument())
	                    {
	                        iControl->iContentDimension.iHeight = iSvgModule->GetViewportHeight(iSvgModule->SvgDocument());
	                    }
	                    else
	                    {
	                        return;
	                    }
                    }
                    else
                    {
                        heightInPercentage = ETrue;
                    }
                    // After loading the content, the width & height will be visible to plugin.
                    // It is time to adjust the size if needed.
                    if ( AdjustDimention(widthInPercentage,heightInPercentage) )
                        {
                        // dimention change is needed so return and notify Browser.
                        return;
                        }
                    iSvgModule->Start();
                    		iStartCalled = ETrue;
            } 
           
	}


CSvgtPluginControl* CSvgtPlugin::Control()
    {
    return iControl;
    }

NPP CSvgtPlugin::Instance()
    {
    return iInstance;
    }

void  CSvgtPlugin::GetSizeOfRenderBuffer(TSize& aSize)
	{
	if ( iRenderBuffer )
		{
		aSize = iRenderBuffer->SizeInPixels();
		}
	else
		{
		aSize = TSize ( 0, 0 );
		}
	}



/**********************************************************************/
//					implements MSvgRequestObserver
/**********************************************************************/

void CSvgtPlugin::UpdateScreen()
	{
	iControl->DrawNow();
	}


TBool CSvgtPlugin::ScriptCall( const TDesC& /*aScript*/,
                                  CSvgElementImpl* /*aCallerElement*/ )
	{
    return EFalse;
	}

TBool CSvgtPlugin::LinkEntered( const TDesC& /* aUri */ )
{
    // update mouse pointer here
    return ETrue;
}

TBool CSvgtPlugin::LinkExited( const TDesC& /* aUri */ )
{
    // update mouse pointer here
    return ETrue;
}
TBool CSvgtPlugin::LinkActivated( const TDesC& aUrl )
	{
	CSvgtPluginEcomMain* lSvgtPluginEcomMain = (CSvgtPluginEcomMain*) Dll :: Tls ();
	NPNetscapeFuncs* lNetscapeFuncs = lSvgtPluginEcomMain->Funcs();
	if(lNetscapeFuncs)
		{
        TPtrC lTarget('\0',0);
        lTarget.Set(_L("_parent"));
        lNetscapeFuncs->geturl(iInstance,aUrl,&lTarget);
   		}
	return ETrue;
	}

void CSvgtPlugin::VolumeChanged(TInt aNewValue)
    {
   	RDebug::Print(_L("Volume level: %d"), aNewValue);    
    aNewValue *= 10; //to make the volume in percentage
    iSvgModule->SetAudioVolume( aNewValue );
    }
    
TInt CSvgtPlugin::FetchImage(const TDesC&  aUri,  RFs& /*aSession*/, RFile& /*aFileHandle*/ )
{
    TRAPD(err,FetchImageL(aUri));
    
    return err;
    
}
    
void CSvgtPlugin::FetchImageL(const TDesC&  aUri)    
    {
        TInt    lCount     = iImageUrlInfoList.Count();
        HBufC*  lListItem  = NULL;
        HBufC*  lUrl       = NULL;
                  
        //checks whether the request is already made for same Uri.        
        for(TInt i = 0; i < lCount; i++)
        {
            lListItem = HBufC::NewLC(iImageUrlInfoList[i]->Length());
            lUrl      = aUri.AllocLC();
                 
            *lListItem = *iImageUrlInfoList[i];
            
            // Convert the List Item to Lower case
            TPtr lListItemPtr = lListItem->Des();
            lListItemPtr.LowerCase();
            
            // Convert the Url to Lower case
            TPtr lUrlPtr = lUrl->Des();
            lUrlPtr.LowerCase();
            
            if( lListItemPtr.Compare(lUrlPtr) == 0 )       	
            {
                CleanupStack::PopAndDestroy(2);
                
                lListItem = NULL;
                lUrl      = NULL;
               	// Same Url is already requested in the past, no need to continue.
              	User::Leave(KErrAlreadyExists); // Indicates the download is pending
            }
            
            CleanupStack::PopAndDestroy(2);
            
            lListItem = NULL;
            lUrl      = NULL;
        }
        
         
        // --------------------------------------
		// Request file from browser
		// --------------------------------------
		CSvgtPluginEcomMain* lSvgtPluginEcomMain = (CSvgtPluginEcomMain*) Dll :: Tls ();
		NPNetscapeFuncs* lNetscapeFuncs = lSvgtPluginEcomMain->Funcs();
		if(lNetscapeFuncs)
		{
			TPtrC lTarget('\0',0);
			
			lUrl = aUri.AllocL();
			
			// Current count is used as a context data.
			void* contextptr = (void*)(lCount);   
			NPError error = lNetscapeFuncs->geturlnotify(iInstance,aUri,NULL,contextptr);		
			    
			// Add the Url and RequestID to the List
			iImageUrlInfoList.Append(lUrl);
			    
			if ( error != 0 )
    		{
                #ifdef _DEBUG
                		lNetscapeFuncs->status(iInstance,_L("Image failed"));
                #endif
                
                User::LeaveIfError(error);
    		}
			#ifdef _DEBUG
            lNetscapeFuncs->status(iInstance,_L("Downloading Image"));
            #endif
		}
	
        
        
        User::Leave(KRequestPending); // Indicate the download is pending
    }

TInt CSvgtPlugin::FetchFont( const TDesC& /* aUri */,  RFs& /* aSession */, RFile&  /* aFileHandle */)
    {
	// Fix for ANAE-775B7E. There is a limitaiton. For rendering text with 
	// external font synchronous fetching of the file is needed. But the 
	// external font file is fetched asynchronously by browser when we say 
	// geturlnotify(). There is no API to pass the font data after fetching
	// asynchronously. Hence the code here is removed by returning an error.
	return KErrNotFound;
    }


TBool CSvgtPlugin::LinkActivatedWithShow(const TDesC& aUri,const TDesC& /*aShow*/)
	{
	CSvgtPluginEcomMain* lSvgtPluginEcomMain = (CSvgtPluginEcomMain*) Dll :: Tls ();
	NPNetscapeFuncs* lNetscapeFuncs = lSvgtPluginEcomMain->Funcs();
	if(lNetscapeFuncs)
		{
        TPtrC lTarget('\0',0);
        lTarget.Set(_L("_parent"));
		lNetscapeFuncs->geturl(iInstance,aUri,&lTarget);
		}
	return ETrue;
	}

void CSvgtPlugin::GetSmilFitValue(TDes16 &aSmilValue)
    {
    aSmilValue.Copy(_L("meet"));
    }

void CSvgtPlugin::UpdatePresentation(const TInt32&  aNoOfAnimation)
	{
 	if (iTotalNumerOfAnimation < 0)
  		{
 		if (aNoOfAnimation != 0)
			{
			iStaticContent = EFalse;
			iTotalNumerOfAnimation = aNoOfAnimation;
			}
		else
			{
			iStaticContent = ETrue;
			}
		}
 	else
		{
		iTotalNumerOfAnimation--;
		}
	}

void CSvgtPlugin::GetViewPort(TInt /*getWidth*/, TInt /*getHeight*/, TBool /*isWidthInPer*/, TBool /*isHeightInPer*/, TInt &setWidth, TInt &setHeight)
{
   
    setWidth = iRenderWidth;    
    setHeight = iRenderHeight;    
    
    iViewPortWidth = setWidth;
    iViewPortHeight = setHeight;
}
// ---------------------------------------------------------------------------
// This function extracts attributes from the list that is passed from Browser.
// Use to check if width & height are defined in XHTML tag
// ---------------------------------------------------------------------------
//
NPError CSvgtPlugin::SvgPluginNew(NPMIMEType /*pluginType*/, NPP instance,
    uint16 /*mode*/, CDesC16Array* argn, CDesC16Array* argv, NPSavedData* )
    {
    if (instance->pdata != this)
        {
        return NPERR_INVALID_INSTANCE_ERROR;
        }

    for (TInt i = 0; i < argn->Count(); i++)
        {
        TPtrC16 namePtr((*argn)[i]);
        TPtrC16 valPtr((*argv)[i]);

        if (KWidth().FindF(namePtr) == 0)
            {
            TLex lString( valPtr );
            lString.SkipSpace();
            TInt result = lString.Val(iRenderWidth);
            // Check if object width was specified as
            // percentage value.
            if ( valPtr.Locate( '%' ) != KErrNotFound )
                {
                // Do not store the value, browser takes
                // care of computing the width
            	iObjWidthInPercentage = ETrue; 
                }
            if (( result != KErrNone) || (iRenderWidth < 0))
                {
                iObjectWidth = 0;
                }
            else
            	{
	            iObjectWidth = iRenderWidth;
            	}
            iObjectWidthSpecified = ETrue;
            }
        else if (KHeight().FindF(namePtr) == 0)
            {
            TLex lString( valPtr );
            lString.SkipSpace();
            TInt result = lString.Val(iRenderHeight);
            // Check if object height was specified as a
            // percentage value.
            if ( valPtr.Locate( '%' ) != KErrNotFound )
                {
                // Do not store the value, browser takes
                // care of computing the height                
            	iObjHeightInPercentage = ETrue; 
                }
            if (( result != KErrNone) || ( iRenderHeight < 0 ))
                {
                iObjectHeight = 0;
                }
            else
           		{
	            iObjectHeight = iRenderHeight;
           		}
           	iObjectHeightSpecified = ETrue ;
            }
        }

    return NPERR_NO_ERROR;
    }


// ---------------------------------------------------------------------------
// ResizeWindow() This function is to notify that a new set of dimention value
// should be taken into consideration. The new w&h is determined from
// AdjustDimention().
//
// ---------------------------------------------------------------------------
//
void CSvgtPlugin::ResizeWindow(TInt aWidth, TInt aHeight)
    {
    NPPVariable variable;
	CSvgtPluginEcomMain* lSvgtPluginEcomMain = (CSvgtPluginEcomMain*) Dll::Tls();
	NPNetscapeFuncs* lNetscapeFuncs = lSvgtPluginEcomMain->Funcs();
	NPWindow newWindow;
    variable = NPPVpluginWindowSize;
    newWindow.x = iCurrentWindow.x;
    newWindow.y = iCurrentWindow.y;
    newWindow.window = iCurrentWindow.window;
    newWindow.clipRect = iCurrentWindow.clipRect;
    newWindow.type = iCurrentWindow.type;
    newWindow.width = aWidth;
    newWindow.height = aHeight;
    TInt err = lNetscapeFuncs->setvalue(iInstance, variable, (void*)&newWindow);
    }


// ---------------------------------------------------------------------------
// AdjustDimention() This function does the logic to check if width & height
// is needed to be adjusted
// ---------------------------------------------------------------------------
//
TBool CSvgtPlugin::AdjustDimention( TBool aWidthInPercentage, TBool aHeightInPercentage)
    {
    iSvgModule->AddListener( static_cast < MSvgViewPortListener*>(this), ESvgViewPortListener );
    if ( iAdjusted )
    	return EFalse;
    
    // we set the width height in all cases except when width/height has been
    // explicitly specified in the browser hence its set here for all cases
    iAdjusted = ETrue;
   // html provides absolute width, no height
   // svg provides absolute width and absolute height
   if (
   ( iObjectWidthSpecified && !iObjWidthInPercentage && !iObjectHeightSpecified ) 
   && ( !aWidthInPercentage && !aHeightInPercentage)
   )
        {
        // only iObjectWidth is valid
        // Adjust missing object value as per aspect ratio of 
        // svg's viewport
        if(iControl->iContentDimension.iWidth)
        {
            TReal newHeight = (TInt)((TReal32)iObjectWidth *
            ( (TReal32)iControl->iContentDimension.iHeight /
                (TReal32)iControl->iContentDimension.iWidth));
            
            ResizeWindow(iObjectWidth, newHeight);
        
        }
        return ETrue;
        }
    
   // html provides no width and absolute height
   // svg provides absolute width and absolute height
   else if (
    ( !iObjectWidthSpecified && iObjectHeightSpecified && !iObjHeightInPercentage ) 
    && ( !aWidthInPercentage && !aHeightInPercentage)
    )
        {
        // only iObjectHeight is valid
        // Adjust missing object value as per aspect ratio of 
        // svg's viewport
        if(iControl->iContentDimension.iHeight)
        {
            TInt newWidth = (TInt)((TReal32)iObjectHeight *
            ( (TReal32)iControl->iContentDimension.iWidth /
                (TReal32)iControl->iContentDimension.iHeight));
            ResizeWindow(newWidth, iObjectHeight);
            
        }
        return ETrue;
        }
   // html provides no width and no height
   // svg provides absolute width and absolute height
   else if(
    (!iObjectWidthSpecified && !iObjectHeightSpecified)
     && ( !aWidthInPercentage && !aHeightInPercentage)
     )
        {
        ResizeWindow(iControl->iContentDimension.iWidth, iControl->iContentDimension.iHeight);
        return ETrue;
        }
   // html provides width and height be it percentages or absolute values
   else if(iObjectWidthSpecified && iObjectHeightSpecified)
        {
        iAdjusted = EFalse;
        return EFalse;
        }
    else
        {
        TReal newWidth = 0.0f;
        TReal newHeight = 0.0f;
        CEikonEnv* lEnv = STATIC_CAST( CEikonEnv*, iControl->ControlEnv() );
        
        TRect lClientRect = TRect( TPoint( 0,0 ), lEnv->EikAppUi()->ClientRect().Size());
       
       // if the browser has specified a valid absolute value width
        if ( !iObjWidthInPercentage && iObjectWidthSpecified)
            {
            newWidth = iObjectWidth; 
            }
        // if the browser has specified a valid percentage value width
        else if (iObjWidthInPercentage && iObjectWidthSpecified )
            {
            // Compute the new dimension by scaling clientRect dimension
            newWidth = lClientRect.Width() * iObjectWidth / 100;
            }
        // if the browser has specified a valid absolute value height
        if ( !iObjHeightInPercentage && iObjectHeightSpecified)
            {
            newHeight = iObjectHeight;
            }
        // if the browser has specified a valid percentage value Height
        else if(iObjHeightInPercentage && iObjectHeightSpecified)
            {
            // Compute the new dimension by scaling clientRect dimension
            newHeight = lClientRect.Height() * iObjectHeight / 100;
            }
            
        // if the browser has not specified the width
        if(!iObjectWidthSpecified)
            {
            if(aWidthInPercentage)
                {
                newWidth = lClientRect.Width() * iControl->iContentDimension.iWidth / 100;
                }
            else
                {
                newWidth = iControl->iContentDimension.iWidth;
                }
            }
        // if the browser has not specified the height
        if(!iObjectHeightSpecified)
            {
            if(aHeightInPercentage)
                {
                newHeight = lClientRect.Height() * iControl->iContentDimension.iHeight / 100;
                }
            else
                {
                newHeight = iControl->iContentDimension.iHeight;
                }
            }
        ResizeWindow(newWidth, newHeight);
        return ETrue;   
        }
    }

TBool CSvgtPlugin::LoadSvgFile( const TDesC& aFilename )
	{
	if ( !iSvgModule )
		{
		return EFalse;
		}
	MSvgError* error = iSvgModule->Load( aFilename );
	if ( !error )
		{
		return EFalse;
		}
	
	// Loading error
    if ( error->HasError() && !error->IsWarning() )
        {
    	return EFalse;
        }
    TInt currentVolume;
    if (FeatureManager::FeatureSupported(KFeatureIdSideVolumeKeys))
        {
        currentVolume = iKeyListener->GetCurrentVolume();
   	    }
   	else
   	    {
        currentVolume = iRepositoryListener->GetCurrentVolume();
   	    }
   	    
   	VolumeChanged(currentVolume );
    iTotalNumerOfAnimation = -1;
    return ETrue;
	}

