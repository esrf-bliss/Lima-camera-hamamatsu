Hamamatsu Tango device
=====================

This is the reference documentation of the Hamamatsu Tango device.

you can also find some useful information about prerequisite/installation/configuration/compilation in the :ref:`Hamamatsu camera plugin <camera-hamamatsu>` section.

Properties
----------

================= =============== =============== ===========================================================================
Property name	  Mandatory	  Default value	  Description
================= =============== =============== ===========================================================================
camera_number	  No		  0               The camera number,  default is  0	
frame_buffer_size No              10              The DCAM frame buffer size used during the acquisition (see dcambuf_alloc)
readout_speed     No              normal          The readout speed, normal/slow (if supported by the camera model)
================= =============== =============== ===========================================================================



Attributes
----------
======================= ======= ======================= ======================================================================
Attribute name		RW	Type			Description
======================= ======= ======================= ======================================================================
frame_rate              ro      DevDouble               The last computed frame per second (the value is computed every
                                                        100 frames only)
lost_frames             ro      DevLong                 Number of frames lost during the current or last acquisition
readout_speed           rw      DevString               The readout speed, normal/slow **(\*)**
                                                         - **SLOW**
                                                         - **NORMAL**
sensor_temperature      ro      DevDouble               Sensor temperature (if supported by the camera model)
======================= ======= ======================= ======================================================================

**(\*)** Use the command getAttrStringValueList to get the list of the supported value for these attributes. 


Commands
--------

=======================	=============== =======================	===========================================
Command name		Arg. in		Arg. out		Description
=======================	=============== =======================	===========================================
Init			DevVoid 	DevVoid			Do not use
State			DevVoid		DevLong			Return the device state
Status			DevVoid		DevString		Return the device state as a string
getAttrStringValueList	DevString:	DevVarStringArray:	Return the authorized string value list for
			Attribute name	String value list	a given attribute name
=======================	=============== =======================	===========================================

