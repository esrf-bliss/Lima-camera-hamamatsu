// console/unpack_mono12
//

#include "../misc/console4.h"
#include "../misc/common.h"

void unpack_mono12_image( void* pSrcTop, int32 srcRowbytes, void* pDstTop, int32 dstRowbytes, int32 width, int32 height )
{
	WORD lut1[65536];
	WORD lut2[65536];

	// make lut to unpack MONO12
	int i, j;
	for( i=0; i<65536; i++ )
	{
		WORD w = (WORD)i;
		BYTE* p = (BYTE*)&w;

		lut1[i] = (p[0] << 4) + (p[1] & 0x0F);
		lut2[i] = (p[1] << 4) + ((p[0] & 0xF0) >> 4);
	}

	// unpack MONO12 and copy
	char* src = (char*)pSrcTop;
	char* dst = (char*)pDstTop;

	for( i=0; i<height; i++ )
	{
		WORD* pDst = (WORD*)(dst + dstRowbytes * i);
		BYTE* pSrc = (BYTE*)(src + srcRowbytes * i);
		for( j=0; j<width/2; j++ )
		{
			*pDst++ = lut1[*(WORD*)pSrc++];
			*pDst++ = lut2[*(WORD*)pSrc++];
			pSrc++;
		}
	}
}

void sample_access_mono12_image( HDCAM hdcam )
{
	DCAMERR err;

	// transferinfo param
	DCAMCAP_TRANSFERINFO transferinfo;
	memset( &transferinfo, 0, sizeof(transferinfo) );
	transferinfo.size	= sizeof(transferinfo);

	// get number of captured image
	err = dcamcap_transferinfo( hdcam, &transferinfo );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamcap_transferinfo()" );
		return;
	}

	if( transferinfo.nFrameCount < 1 )
	{
		printf( "not capture image\n" );
		return;
	}

	// prepare frame param
	DCAMBUF_FRAME	bufframe;
	memset( &bufframe, 0, sizeof(bufframe) );
	bufframe.size	= sizeof(bufframe);
	bufframe.iFrame	= transferinfo.nNewestFrameIndex;

	// access image
	err = dcambuf_lockframe( hdcam, &bufframe );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcambuf_lockframe()" );
		return;
	}

	if( bufframe.type != DCAM_PIXELTYPE_MONO12 )
	{
		printf( "not MONO12 image.\n" );
		return;
	}

	int32 rowbytes		= bufframe.width * 2;
	int32 framebytes	= rowbytes * bufframe.height;
	char* pImage = new char[ framebytes ];
	memset( pImage, 0, framebytes );

	unpack_mono12_image( bufframe.buf, bufframe.rowbytes, pImage, rowbytes, bufframe.width, bufframe.height );
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

			// set mono12
			err = dcamprop_setvalue( hdcam, DCAM_IDPROP_IMAGE_PIXELTYPE, DCAM_PIXELTYPE_MONO12 );
			if( !failed(err) )
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

						// access image
						sample_access_mono12_image( hdcam );
					}

					// release buffer
					dcambuf_release( hdcam );
				}
			}
			else
				dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP=IMAGE_PIXELTYPE, VALUE=DCAM_PIXELTYPE_MONO12" );

			// close wait handle
			dcamwait_close( hwait );
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