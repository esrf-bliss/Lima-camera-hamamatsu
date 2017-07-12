// console/splitview
//

#include "../misc/console4.h"
#include "../misc/common.h"

#define	USE_FRAMEBUNDLE	0	// use frame bundle

BOOL set_subarray_splitview( HDCAM hdcam, int32 width, int32 height, int32 v1_hoffset, int32 v2_hoffset, int32 v1_voffset, int32 v2_voffset )
{
	DCAMERR err;

	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYMODE, DCAMPROP_MODE__OFF );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SUBARRAYMODE, VALUE=OFF" );
		return FALSE;
	}

	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYHSIZE, width );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SUBARRAYHSIZE, VALUE=%d", width );
		return FALSE;
	}

	if( v1_hoffset = v2_hoffset )
	{
		err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYHPOS, v1_hoffset );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SUBARRAYHPOS, VALUE=%d", v1_hoffset );
			return FALSE;
		}
	}
	else
	{
		err = dcamprop_setvalue( hdcam, DCAM_IDPROP_VIEW_(1, DCAM_IDPROP_SUBARRAYHPOS), v1_hoffset );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SUBARRAYHPOS_VIEW1, VALUE=%d", v1_hoffset );
			return FALSE;
		}

		err = dcamprop_setvalue( hdcam, DCAM_IDPROP_VIEW_(2, DCAM_IDPROP_SUBARRAYHPOS), v2_hoffset );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SUBARRAYHPOS_VIEW2, VALUE=%d", v2_hoffset );
			return FALSE;
		}
	}

	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYVSIZE, height );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SUBARRAYVSIZE, VALUE=%d",height );
		return FALSE;
	}

	if( v1_voffset == v2_voffset )
	{
		err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYVPOS, v1_voffset );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SUBARRAYVPOS, VALUE=%d", v1_voffset );
			return FALSE;
		}
	}
	else
	{
		err = dcamprop_setvalue( hdcam, DCAM_IDPROP_VIEW_(1, DCAM_IDPROP_SUBARRAYVPOS), v1_voffset );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SUBARRAYVPOS_VIEW1, VALUE=%d", v1_voffset );
			return FALSE;
		}

		err = dcamprop_setvalue( hdcam, DCAM_IDPROP_VIEW_(2, DCAM_IDPROP_SUBARRAYVPOS), v2_voffset );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SUBARRAYVPOS_VIEW2, VALUE=%d", v2_voffset );
			return FALSE;
		}
	}

	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYMODE, DCAMPROP_MODE__ON );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SUBARRAYMODE, VALUE=ON" );
		return FALSE;
	}

	return TRUE;
}

void get_image_maxsize( HDCAM hdcam, int32& hmax, int32& vmax )
{
	DCAMERR err;
	
	DCAMPROP_ATTR attr;
	memset( &attr, 0, sizeof(attr) );
	attr.cbSize	= sizeof(attr);

	attr.iProp	= DCAM_IDPROP_SUBARRAYHSIZE;

	err = dcamprop_getattr( hdcam, &attr );	ASSERT( !failed(err) );
	hmax = (int32)attr.valuemax;

	attr.iProp	= DCAM_IDPROP_SUBARRAYVSIZE;

	err = dcamprop_getattr( hdcam, &attr );	ASSERT( !failed(err) );
	vmax = (int32)attr.valuemax;
}

void access_image( HDCAM hdcam )
{
	DCAMERR err;

	// prepare frame param
	DCAMBUF_FRAME frame;
	memset( &frame, 0, sizeof(frame) );
	frame.size		= sizeof(frame);
	frame.iFrame	= 0;

	// access image
	err = dcambuf_lockframe( hdcam, &frame );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcambuf_lockframe()" );
		return;
	}

	double v;

	int32 nView = 0;
	err = dcamprop_getvalue( hdcam, DCAM_IDPROP_NUMBEROF_VIEW, &v );
	if( failed(err) || v < 2 )
		return;

	nView = (int32)v;	ASSERT( nView == 2 );

	char* pBuf[2];
	memset( pBuf, 0, sizeof(pBuf) );

	int32 pFramebytes[2];
	memset( pFramebytes, 0, sizeof(pFramebytes) );

#if USE_FRAMEBUNDLE
	int32 nBundle = 1;
	BOOL bHasViewBundleInfo = FALSE;
	err = dcamprop_getvalue( hdcam, DCAM_IDPROP_FRAMEBUNDLE_MODE, &v );
	if( !failed(err) )
	{
		err = dcamprop_getvalue( hdcam, DCAM_IDPROP_FRAMEBUNDLE_NUMBER, &v );	ASSERT( !failed(err) );
		nBundle = (int32)v;

		DCAMPROP_ATTR attr;
		memset( &attr, 0, sizeof(attr) );
		attr.cbSize	= sizeof(attr);
		attr.iProp	= DCAM_IDPROP_FRAMEBUNDLE_ROWBYTES;

		err = dcamprop_getattr( hdcam, &attr );	ASSERT( !failed(err) );
		if( attr.attribute & DCAMPROP_ATTR_HASVIEW )
			bHasViewBundleInfo = TRUE;
	}
	else
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_getvalue()", "IDPROP=FRAMEBUNDLE_MODE" );
		return;
	}
#endif

	// copy each view image
	int i;
	for( i=1; i<=nView; i++ )
	{
		err = dcamprop_getvalue( hdcam, DCAM_IDPROP_VIEW_(i, DCAM_IDPROP_IMAGE_FRAMEBYTES), &v );	ASSERT( !failed(err) );
		pFramebytes[i-1] = (int32)v;
		pBuf[i-1] = new char[ pFramebytes[i-1] ];
		memset( pBuf[i-1], 0, pFramebytes[i-1] );

		err = dcamprop_getvalue( hdcam, DCAM_IDPROP_VIEW_(i, DCAM_IDPROP_IMAGE_TOPOFFSETBYTES), &v );	ASSERT( !failed(err) );
		int32 viewoffset = (int32)v;

#if USE_FRAMEBUNDLE
		int32 bundle_rowbytes = 0;
		int32 bundle_framestepbytes = 0;
		if( bHasViewBundleInfo )
		{
			err = dcamprop_getvalue( hdcam, DCAM_IDPROP_VIEW_(i, DCAM_IDPROP_FRAMEBUNDLE_ROWBYTES), &v );	ASSERT( !failed(err) );
			bundle_rowbytes = (int32)v;

			err = dcamprop_getvalue( hdcam, DCAM_IDPROP_VIEW_(i, DCAM_IDPROP_FRAMEBUNDLE_FRAMESTEPBYTES), &v );	ASSERT( !failed(err) );
			bundle_framestepbytes = (int32)v;
		}
		else
		{
			err = dcamprop_getvalue( hdcam, DCAM_IDPROP_FRAMEBUNDLE_ROWBYTES, &v );	ASSERT( !failed(err) );
			bundle_rowbytes = (int32)v;

			err = dcamprop_getvalue( hdcam, DCAM_IDPROP_FRAMEBUNDLE_FRAMESTEPBYTES, &v );	ASSERT( !failed(err) );
			bundle_framestepbytes = (int32)v;
		}

		int j;
		for( j=0; j<nBundle; j++ )
		{
			int32 srcoffset = viewoffset + j * bundle_framestepbytes;
			char* pSrc = (char*)frame.buf + srcoffset;
			char* pDst = pBuf[i-1] + j * frame.rowbytes * frame.height;

			int k;
			for( k=0; k<frame.height; k++ )
			{
				memcpy_s( _secure_ptr(pDst, pFramebytes[i-1] - (pDst - pBuf[i-1])), pSrc, frame.rowbytes );

				pSrc += bundle_rowbytes;
				pDst += frame.rowbytes;
			}
		}
#else
		char* pSrc = (char*)frame.buf + viewoffset;
		memcpy_s( _secure_ptr( pBuf[i-1], pFramebytes[i-1] ), pSrc, pFramebytes[i-1] );
#endif
	}

	{
		// TODO: add your process to each view
	}

	if( pBuf[0] != NULL )
		delete  pBuf[0];

	if( pBuf[1] != NULL )
		delete  pBuf[1];
}

int main( int argc, char* const argv[] )
{
	printf( "PROGRAM START\n" );

	int	ret = 0;

	DCAMERR err;

	// initialize DCAM-API and open device
	HDCAM hdcam;
	hdcam = dcamcon_init_open();
	if( hdcam != NULL )
	{
		// show device information
		dcamcon_show_dcamdev_info( hdcam );

		// set split view mode
		err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SENSORMODE, DCAMPROP_SENSORMODE__SPLITVIEW );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=SENSORMODE, VALUE=SPLITVIEW" );
			ret = 1;
		}
		else
		{
			// open wait handle
			DCAMWAIT_OPEN	waitopen;
			memset( &waitopen, 0, sizeof(waitopen) );
			waitopen.size	= sizeof(waitopen);
			waitopen.hdcam	= hdcam;

			err = dcamwait_open( &waitopen );
			if( failed(err) )
			{
				dcamcon_show_dcamerr( hdcam, err, "dcamwait_open()" );
				ret = 1;
			}
			else
			{
				HDCAMWAIT hwait = waitopen.hwait;

				int32 hmax, vmax;
				get_image_maxsize( hdcam, hmax, vmax );
				
				int32 width		= hmax / 4;
				int32 height	= vmax / 4;
				
				// set subarray to each view
				if( set_subarray_splitview( hdcam, width, height, width/2, width/2, height/2, height/2 ) )
				{
#if USE_FRAMEBUNDLE
					int32 nBundle = vmax/ height;
					
					// set frame bundle
					err = dcamprop_setvalue( hdcam, DCAM_IDPROP_FRAMEBUNDLE_NUMBER, nBundle );	ASSERT( !failed(err) );
					err = dcamprop_setvalue( hdcam, DCAM_IDPROP_FRAMEBUNDLE_MODE, DCAMPROP_MODE__ON );
					if( failed(err) )
					{
						dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=FRAMEBUNDLE_MODE, VALUE=ON" );
						ret = 1;
					}
					else
#endif
					{
						// allocate buffer
						int32 number_of_buffer = 10;
						err = dcambuf_alloc( hdcam, number_of_buffer );
						if( failed(err) )
						{
							dcamcon_show_dcamerr( hdcam, err, "dcambuf_alloc()" );
							ret = 1;
						}
						else
						{
							// start capture
							err = dcamcap_start( hdcam, DCAMCAP_START_SEQUENCE );
							if( failed(err) )
							{
								dcamcon_show_dcamerr( hdcam, err, "dcamcap_start()" );
								ret = 1;
							}
							else
							{
								printf( "\nStart Capture\n" );

								// set wait param
								DCAMWAIT_START waitstart;
								memset( &waitstart, 0, sizeof(waitstart) );
								waitstart.size		= sizeof(waitstart);
								waitstart.eventmask	= DCAMWAIT_CAPEVENT_FRAMEREADY;
								waitstart.timeout	= 1000;

								err = dcamwait_start( hwait, &waitstart );
								if( failed(err) )
								{
									dcamcon_show_dcamerr( hdcam, err, "dcamwait_start()" );
									ret = 1;
								}

								// stop capture
								dcamcap_stop( hdcam );
								printf( "Stop Capture\n" );
							}

							// access image
							access_image( hdcam );

							// release buffer
							dcambuf_release( hdcam );
						}
					}
				}

				// close wait handle
				dcamwait_close( hwait );
			}
		}

		// close DCAM handle
		dcamdev_close( hdcam );
	}
	else
	{
		ret = 1;
	}

	// finalize DCAM-API
	dcamapi_uninit();

	printf( "PROGRAM END\n" );
	return ret;
}