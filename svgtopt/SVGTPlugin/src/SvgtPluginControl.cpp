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

#include <featmgr.h>
#include <eikenv.h>
#include <w32std.h>
#include <e32base.h>
#include <barsread.h>
#include <SVGEngineInterfaceImpl.h>
//#include <Svgengineinterface.h>
#include <f32file.h>

#include "Svgtplugin.h"

#define SVGT_PLUGIN_DEVICE_KEY_0 48
#define SVGT_PLUGIN_DEVICE_KEY_1 49
#define SVGT_PLUGIN_DEVICE_KEY_2 50
#define SVGT_PLUGIN_DEVICE_KEY_3 51
#define SVGT_PLUGIN_DEVICE_KEY_4 52
#define SVGT_PLUGIN_DEVICE_KEY_5 53
#define SVGT_PLUGIN_DEVICE_KEY_6 54
#define SVGT_PLUGIN_DEVICE_KEY_7 55
#define SVGT_PLUGIN_DEVICE_KEY_8 56
#define SVGT_PLUGIN_DEVICE_KEY_9 57
#define SVGT_PLUGIN_DEVICE_KEY_ASTERIX 42


void CSvgtPluginControl::ConstructL(CSvgtPlugin* aPlugin, const CCoeControl* aParentControl)
    {
    iAsFileCalled = EFalse;
    iShowCursor = EFalse;
    iPointerX = 5;
    iPointerY = 5;
    iPlugin=aPlugin;
    this->CreateWindowL(aParentControl);
    ActivateL();
    EnableDragEvents();
    iLightObserver = CHWRMLight::NewL(this);
    }

CSvgtPluginControl::~CSvgtPluginControl()
    {
	delete iLightObserver;
    }

void CSvgtPluginControl::Draw(const TRect& aRect) const
    {
    CWindowGc &gc = SystemGc();
    if (iPlugin && iPlugin->iRenderBuffer !=NULL)
        {
        
        // Fix for the iframe scrolling.
    	TRect lCliprect;
    	
    	lCliprect.iTl.iY = iPlugin->iCurrentWindow.clipRect.top;
    	lCliprect.iTl.iX = iPlugin->iCurrentWindow.clipRect.left;
    	lCliprect.iBr.iY = iPlugin->iCurrentWindow.clipRect.bottom;
    	lCliprect.iBr.iX = iPlugin->iCurrentWindow.clipRect.right;
		
		TSize lWindowSize(iPlugin->iCurrentWindow.width,iPlugin->iCurrentWindow.height);
		
		if(lWindowSize!=(lCliprect.Size()))
			{
			// Get the Abs(x,y) of the window and render with the size of the cliprect 
			TPoint lpt(Abs(iPlugin->iCurrentWindow.x),Abs(iPlugin->iCurrentWindow.y ));
			TRect lrect(lpt,lCliprect.Size());
			gc.BitBlt( aRect.iTl,  (iPlugin->iRenderBuffer), lrect );
			}
        else
        	{
        	gc.BitBlt( aRect.iTl,  (iPlugin->iRenderBuffer), aRect );
        	}
        

		if ( iShowCursor )
			{
	        TInt lX = iPointerX + aRect.iTl.iX;
	        TInt lY = iPointerY + aRect.iTl.iY;
	        TRect   csrh    ( lX - 2,
	                          lY - 5,
	                          lX + 2,
	                          lY + 5 );
	        TRect   csrv    ( lX - 5,
	                          lY - 2,
	                          lX + 5,
	                          lY + 2 );
	        gc.SetPenSize( TSize( 1, 1 ) );
	        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	        gc.SetBrushColor( TRgb( 0, 0, 0 ) );
	        gc.SetPenColor( TRgb( 0, 0, 0 ) );
	        gc.DrawRect( csrh );
	        gc.DrawRect( csrv );
	        csrh.Shrink( 1, 1 );
	        csrv.Shrink( 1, 1 );
	        gc.SetBrushColor( TRgb( 255, 225, 225 ) );
	        gc.SetPenColor( TRgb( 255, 225, 225 ) );
	        gc.DrawRect( csrh );
	        gc.DrawRect( csrv );
			}
        }
    }

TInt CSvgtPluginControl::CountComponentControls() const
    {
    return 0;
    }

CCoeControl* CSvgtPluginControl::ComponentControl(TInt /*aIndex*/) const
    {
    return NULL;
    }

void CSvgtPluginControl::SizeChanged()
    {
    }

#ifndef ER5
TCoeInputCapabilities CSvgtPluginControl::InputCapabilities() const
    {
    return TCoeInputCapabilities(TCoeInputCapabilities::ENavigation);
    }
#endif

void CSvgtPluginControl::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
    {
    }

void CSvgtPluginControl::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    }

TKeyResponse CSvgtPluginControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    TKeyResponse    retVal  = EKeyWasNotConsumed;
    if( iPlugin->iSvgModule && iPlugin->iSvgModule->SvgDocument() )
        {
        if ( aType == EEventKeyUp )
            {
              iPointerDx = 0;
              iPointerDy = 0;
            if ( aKeyEvent.iScanCode == EStdKeyDevice3 ) // Center button of rocker key
               	{
               	iPlugin->iSvgModule->MouseUp( iPointerX, iPointerY );
                retVal = EKeyWasConsumed;
               	} 
			if(iPreviousScanCode.iScanCode == SVGT_PLUGIN_DEVICE_KEY_0)
				{
				   if(!iPreviousScanCode.iRepeats) /*Short Key Press of 'Zero' Do Zoom Out*/
					{
					iPlugin->iSvgModule->Zoom(0.5f);
                    iPlugin->iSvgModule->Redraw();
					}
				   else /*Long Key Press of 'Zero' Reset to Original View*/
					{
                    iPlugin->iSvgModule->OriginalView();
                    iPlugin->iSvgModule->Redraw();
					}
					retVal = EKeyWasConsumed;

				}
			iPreviousScanCode.iRepeats	= 0;
			iPreviousScanCode.iCode		= 0;
			iPreviousScanCode.iScanCode	= 0;
            }
        else if ( aType == EEventKey )
            {
            retVal = EKeyWasConsumed;
            // aKeyEvent.iScanCode does not recognize EStdKeyNkpAsterisk
                switch ( aKeyEvent.iScanCode )
                    {
                case EStdKeyLeftArrow:
                    iPointerDx -= 5;
                    iPointerDy = 0;
                    break;
                case EStdKeyRightArrow:
                    iPointerDx += 5;
                    iPointerDy = 0;
                    break;
                case EStdKeyUpArrow:
                    iPointerDx = 0;
                    iPointerDy -= 5;
                    break;
                case EStdKeyDownArrow:
                    iPointerDx = 0;
                    iPointerDy += 5;
                    break;
                case EStdKeyDevice3:
                    // Center button of rocker key
                    iPlugin->iSvgModule->MouseDown( iPointerX, iPointerY );
                    break;
		       case EStdKeyHash: // Pause/Resume
                    if(iPlugin->iSvgModule->CurrentState() == 0)
                        iPlugin->iSvgModule->Stop();
                    else if(iPlugin->iSvgModule->CurrentState() == 1)
                        iPlugin->iSvgModule->Resume();
                    break;

                case SVGT_PLUGIN_DEVICE_KEY_5: //Zoom In
                    iPlugin->iSvgModule->Zoom(2.0f);
                    iPlugin->iSvgModule->Redraw();
                    break;

                case SVGT_PLUGIN_DEVICE_KEY_1: //Rotate ClockWise  90
                    iPlugin->iSvgModule->Rotate( (float) (3.1415926/2), iContentDimension.iWidth/2, iContentDimension.iHeight/2);
                    iPlugin->iSvgModule->Redraw();
                    break;

                case SVGT_PLUGIN_DEVICE_KEY_3: //Rotate Counter ClockWise  90
                    iPlugin->iSvgModule->Rotate( (float) (-3.1415926/2), iContentDimension.iWidth/2, iContentDimension.iHeight/2);
                    iPlugin->iSvgModule->Redraw();
                    break;
                case SVGT_PLUGIN_DEVICE_KEY_7: //Rotate ClockWise 45
                    iPlugin->iSvgModule->Rotate( (float) (3.1415926/4), iContentDimension.iWidth/2, iContentDimension.iHeight/2);
                    iPlugin->iSvgModule->Redraw();
                    break;
                case SVGT_PLUGIN_DEVICE_KEY_9: //Rotate Counter ClockWise  45
                    iPlugin->iSvgModule->Rotate( (float) (-3.1415926/4), iContentDimension.iWidth/2, iContentDimension.iHeight/2);
                    iPlugin->iSvgModule->Redraw();
                    break;
				case SVGT_PLUGIN_DEVICE_KEY_0:
					iPreviousScanCode = aKeyEvent;
					break;
                default:
                retVal = EKeyWasNotConsumed;
                    }
                }
         
         else if ( aType == EEventKeyDown)
         	{
            // Send the mousedown event for triggering any animation
            // Since the EEventKey is not recieved when MSK is pressed,
            // the mousedown doesn't occur on engine. Hence EEventKey 
            // handled here
            if ( aKeyEvent.iScanCode == EStdKeyDevice3 ) // Center button of rocker key
            	{
            	iPlugin->iSvgModule->MouseDown( iPointerX, iPointerY );
            	retVal=EKeyWasConsumed;
            	}
           	}

        if ( (iPointerDx != 0) || (iPointerDy != 0) )
           {
            UpdatePointer(iPointerDx,iPointerDy);
           }
        }
    return retVal;
    }


void CSvgtPluginControl::AsFile(const TDesC& fname,NPStream* stream)
    {
    // -----------------------------------
	// Check for image was requested
    // -----------------------------------
    if( iPlugin && iFilename.Length() > 0 )
	{
		TInt lFileSize = 0;
        RFile lSvgFile;
        CEikonEnv* iEikEnv = CEikonEnv::Static(); 
        
        // Attempt to open the file in read mode
        User::LeaveIfError( lSvgFile.Open(iEikEnv->FsSession(),fname,EFileRead ) );
        
        // Save on cleanup stack
        CleanupClosePushL( lSvgFile );
        
        // Get the size of the data to create read buffer
        User::LeaveIfError( lSvgFile.Size(lFileSize) );
        
        // Create buffer that will contain the file data
        HBufC8* lFileData = HBufC8::NewLC(lFileSize);
        TPtr8 lFileDataPtr(lFileData->Des());
        
        // Read from the file
        User::LeaveIfError( lSvgFile.Read(lFileDataPtr) );
        
        TInt index = reinterpret_cast<TInt>((stream->notifyData));       
        
        iPlugin->iSvgModule->AssignImageData(*(iPlugin->iImageUrlInfoList[index]), lFileData );
		iPlugin->iSvgModule->Redraw();
		
		// The ownership of lFileData is with the respective Image Element.
		CleanupStack::Pop(1);
		CleanupStack::PopAndDestroy(&lSvgFile); // lSvgFile.Close()
				
		return;
	}
		
	// Svg file
    iFilename.Zero();
    iFilename.Copy(fname);

	if ( ( iPlugin && iPlugin->iSvgModule ) )
        {
        iPlugin->iSvgModule->Stop();
        iPlugin->LoadSvgFile( iFilename );
        // After loading the content, the width & height will be visible to plugin.
        // It is time to adjust the size if needed.
	    TBool widthInPercentage = EFalse;
	    TBool heightInPercentage = EFalse;
	    iContentDimension = iPlugin->iSvgModule->ContentDimensionsInPercentage();
	    
	    if(iContentDimension.iWidth == -1)
	    {
	        widthInPercentage = EFalse;
	        if(iPlugin->iSvgModule->SvgDocument())
	        {
	            iContentDimension.iWidth = iPlugin->iSvgModule->GetViewportWidth(iPlugin->iSvgModule->SvgDocument());    
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
	    
	    if(iContentDimension.iHeight == -1)
	    {
	        heightInPercentage = EFalse;
            if(iPlugin->iSvgModule->SvgDocument())
            {
                iContentDimension.iHeight = iPlugin->iSvgModule->GetViewportHeight(iPlugin->iSvgModule->SvgDocument());    
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
	    if(iPlugin->AdjustDimention(widthInPercentage, heightInPercentage))
            {
            // dimention change is needed so return and notify Browser.
	        iAsFileCalled = ETrue;
            return;
            }

	       	iPlugin->iSvgModule->Start();
        }        
        iAsFileCalled = ETrue;
    }


TBool CSvgtPluginControl::IsAsFileCalled()
    {
    return iAsFileCalled;
    }

TDesC& CSvgtPluginControl::GetFilename()
    {
    return iFilename;
    }


void CSvgtPluginControl::LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus)
{
     //TInt lState = iPlugin->iSvgModule->CurrentState();
	if((aTarget == CHWRMLight::EPrimaryDisplay) || (aTarget == CHWRMLight::EPrimaryDisplayAndKeyboard))
	{
		if(aStatus == CHWRMLight::ELightOff)
		{

			if (iPlugin && iPlugin->iSvgModule)
			{
		    #ifdef _DEBUG
		    RDebug::Printf("Backlight Off");
            #endif //_DEBUG

		    iPlugin->iSvgModule->Pause();
		    }

		}
		else if(aStatus == CHWRMLight::ELightOn)
		{

			if (iPlugin && iPlugin->iSvgModule)
		    {
		    #ifdef _DEBUG
		    RDebug::Printf("Backlight On");
	        #endif //_DEBUG

	        iPlugin->iSvgModule->Resume();
			}
	    }
	}

}
/**
* MOperaPluginNotifier methods
* - Handle notifications browser
*/
TInt CSvgtPluginControl::NotifyL(TNotificationType aCallType, void* aParam)
    {
    if ( (iPlugin == NULL ) ||  ( iPlugin->iSvgModule == NULL ) )
		{
		return KErrNone;
		}

    if(aCallType == EApplicationFocusChanged)
        {
        TInt lState = iPlugin->iSvgModule->CurrentState();
        TBool lInFocus =  (TBool) aParam;

		if ( lInFocus )
	    	{
            // iShowCursor = ETrue;
            if ( lState == 1 )
            	{
		        iPlugin->iSvgModule->Resume();
            	}
			DrawNow();
	    	}
	    else if ( !lInFocus )
	    	{
            //ShowCursor = EFalse;
            if ( lState == 0 )
            	{
		        iPlugin->iSvgModule->Pause();
            	}
			DrawNow();
	    	}
        }
    else if(aCallType == EPluginActivated)
        {
        iShowCursor = ETrue;
        DrawNow();
        }
    else if(aCallType == EPluginDeactivated)
        {
        iShowCursor = EFalse;
        DrawNow();
        }
    else if(aCallType == EPluginInvisible )
        {
        }
    return KErrNone;
    }


void CSvgtPluginControl::ApplicationFocusChanged(TBool /*aInFocus*/)
    {
    }

void CSvgtPluginControl::SystemNotification()
    {
    }

void CSvgtPluginControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    
    
    if ( iPlugin->iSvgModule && iPlugin->iSvgModule->SvgDocument() )
        {
        iPointerDx = 0;
        iPointerDy = 0;
        
        if(aPointerEvent.iType == TPointerEvent::EButton1Down)
            {
            if(aPointerEvent.iPosition.iX <=iPlugin->iRenderWidth && aPointerEvent.iPosition.iY <=iPlugin->iRenderHeight)
                {
                iPointerX = aPointerEvent.iPosition.iX;
                iPointerY = aPointerEvent.iPosition.iY;

				iXPosAtMouseDown = iPointerX;
				iYPosAtMouseDown = iPointerY;
				
                iPlugin->iSvgModule->MouseDown( iPointerX, iPointerY );
                UpdatePointer();
                }
            }
        else if(aPointerEvent.iType == TPointerEvent::EButton1Up)
            {
            iPlugin->iSvgModule->MouseUp( iPointerX, iPointerY );
            DrawDeferred();
            }
         else if (aPointerEvent.iType == TPointerEvent::EDrag)
            {
            if((iPointerX != aPointerEvent.iPosition.iX || iPointerY != aPointerEvent.iPosition.iY) && 
            (aPointerEvent.iPosition.iX <=iPlugin->iRenderWidth && aPointerEvent.iPosition.iY <=iPlugin->iRenderHeight))
                {
                iPointerX = aPointerEvent.iPosition.iX;
                iPointerY = aPointerEvent.iPosition.iY;

                iPlugin->iSvgModule->MouseMove( iPointerX, iPointerY );
                
                UpdatePointer((iXPosAtMouseDown-iPointerX),(iYPosAtMouseDown-iPointerY));
                }
            }
        }
    }

void CSvgtPluginControl::UpdatePointer(
#ifdef __PEN_SUPPORT
        TInt aShiftX , TInt aShiftY
#else
        TInt , TInt
#endif        
    )
    {
    #ifdef __PEN_SUPPORT
     if(FeatureManager::FeatureSupported(KFeatureIdPenSupport))
         {
    if(aShiftX != 0 || aShiftY != 0 )
    {
    	iPlugin->iSvgModule->Pan(-aShiftX,-aShiftY);  
    	iPlugin->iSvgModule->Redraw();  
    	iXPosAtMouseDown = iPointerX;
		iYPosAtMouseDown = iPointerY;	
    	iPlugin->iSvgModule->MouseMove( iPointerX, iPointerY );
        DrawNow();
    }
         }
    #else//_PEN_SUPPORT
     {
     if (iPlugin->iRenderBuffer !=NULL)
        {
        // Due to the iframe scrolling the pointer will run out 
        // of the iframe window when tried to pan inside the plugin. 
        // So the control rect for the pointer is now the cliprect.
        
	//	TRect lControlRect(TSize(iPlugin->iViewPortWidth, iPlugin->iViewPortHeight));
		TRect lControlRect;
    	
    	lControlRect.iTl.iY = iPlugin->iCurrentWindow.clipRect.top;
    	lControlRect.iTl.iX = iPlugin->iCurrentWindow.clipRect.left;
    	lControlRect.iBr.iY = iPlugin->iCurrentWindow.clipRect.bottom;
    	lControlRect.iBr.iX = iPlugin->iCurrentWindow.clipRect.right;
	
        //X value
        iPointerX += iPointerDx;
        if ( iPointerX < 5 )
            {
            iPointerX = 5;
            iPlugin->iSvgModule->Pan( 30, 0 );
            //iPlugin->iSvgModule->Pan( -30, 0 );
            iPlugin->iSvgModule->Redraw();
            }
      else if ( lControlRect.Width() - 5 < iPointerX )
            {
            iPointerX = lControlRect.Width() - 5;
            iPlugin->iSvgModule->Pan( -30, 0 );
            //iPlugin->iSvgModule->Pan( 30, 0 );
            iPlugin->iSvgModule->Redraw();
            }
        //Y value
        iPointerY += iPointerDy;
        if ( iPointerY < 5 )
            {
            iPointerY = 5;
            iPlugin->iSvgModule->Pan( 0, 30 );
            //iPlugin->iSvgModule->Pan( 0, -30 );
            iPlugin->iSvgModule->Redraw();
            }
      else if ( lControlRect.Height() - 5 < iPointerY )
            {
            iPointerY = lControlRect.Height() - 5;
            iPlugin->iSvgModule->Pan( 0,-30 );
            //iPlugin->iSvgModule->Pan( 0,30 );
            iPlugin->iSvgModule->Redraw();
            }
        iPlugin->iSvgModule->MouseMove( iPointerX, iPointerY );
        DrawNow();
        }
     }
    #endif //_PEN_SUPPORT
        
    }

