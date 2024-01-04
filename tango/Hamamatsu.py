############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2023
# European Synchrotron Radiation Facility
# CS40220 38043 Grenoble Cedex 9
# FRANCE
#
# Contact: lima@esrf.fr
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
#=============================================================================
#
# file :        Hamamatsu.py
#
# description : Python source for the Hamamatsu and its commands. 
#                The class is derived from Device. It represents the
#                CORBA servant object which will be accessed from the
#                network. All commands which can be executed on the
#                Pilatus are implemented in this file.
#
# project :     TANGO Device Server
#
# copyleft :    European Synchrotron Radiation Facility
#               BP 220, Grenoble 38043
#               FRANCE
#
#=============================================================================
#         (c) - Bliss - ESRF
#=============================================================================
#
import PyTango
import sys, types, os, time

from Lima import Core
from Lima import Hamamatsu as HamamatsuModule
# import some useful helpers to create direct mapping between tango attributes
# and Lima interfaces.
from Lima.Server import AttrHelper

class Hamamatsu(PyTango.Device_4Impl):

    Core.DEB_CLASS(Core.DebModApplication, 'LimaCCDs')
    
#==================================================================
#   Hamamatsu Class Description:
#
#
#==================================================================

class Hamamatsu(PyTango.Device_4Impl):

#--------- Add you global variables here --------------------------
    Core.DEB_CLASS(Core.DebModApplication, 'LimaCCDs')

#------------------------------------------------------------------
#    Device constructor
#------------------------------------------------------------------
    @Core.DEB_MEMBER_FUNCT
    def __init__(self,cl, name):
        PyTango.Device_4Impl.__init__(self,cl,name)
        # dictionnaries to be used with AttrHelper.get_attr_4u
        self.__ReadoutSpeed = {'SLOW':1,
                               'NORMAL':2}

         #Only needed to map attribute and function which does not fit the with naming convention.
        self.__Attribute2FunctionBase = {
            'frame_rate': 'FPS',
        }

        self.init_device()
                                               
#------------------------------------------------------------------
#    Device destructor
#------------------------------------------------------------------
    def delete_device(self):
        pass


#------------------------------------------------------------------
#    Device initialization
#------------------------------------------------------------------
    @Core.DEB_MEMBER_FUNCT
    def init_device(self):
        self.set_state(PyTango.DevState.ON)

        # Load the properties
        self.get_device_properties(self.get_device_class())

        if self.readout_speed:
            _HamamatsuCamera.setReadoutSpeed(self.__ReadoutSpeed[self.readout_speed.upper()])
        

#==================================================================
#
#    Hamamatsu read/write attribute methods
#
#==================================================================


    def __getattr__(self,name) :
        return AttrHelper.get_attr_4u(self, name, _HamamatsuCamera)


#==================================================================
#
#    Hamamatsu command methods
#
#==================================================================

#------------------------------------------------------------------
#    getAttrStringValueList command:
#
#    Description: return a list of authorized values if any
#    argout: DevVarStringArray   
#------------------------------------------------------------------
    @Core.DEB_MEMBER_FUNCT
    def getAttrStringValueList(self, attr_name):
        return AttrHelper.get_attr_string_value_list(self, attr_name)
    

#==================================================================
#
#    Hamamatsu class definition
#
#==================================================================
class HamamatsuClass(PyTango.DeviceClass):

    #    Class Properties
    class_property_list = {
        }

    #    Device Properties
    device_property_list = {
        'camera_number':
        [PyTango.DevShort,
         'camera number', []],
        'readout_speed':
        [PyTango.DevString,
         'The readout speed, normal/slow', []],
        'frame_buffer_size':
        [PyTango.DevLong,
         'The DCAM frame buffer size used during the acquisition', []],
        }
        

    #    Command definitions
    cmd_list = {
        'getAttrStringValueList':
        [[PyTango.DevString, "Attribute name"],
         [PyTango.DevVarStringArray, "Authorized String value list"]]
        }


    #    Attribute definitions
    attr_list = {
        'sensor_temperature':
        [[PyTango.DevDouble,
          PyTango.SCALAR,
          PyTango.READ],
         {
             'unit': 'C',
             'format': '%f',
             'description': 'Sensor temperature',
             }],
       'readout_speed':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'format': '%d',
             'description': 'readout',
             }],        
       'lost_frames':
        [[PyTango.DevLong,
          PyTango.SCALAR,
          PyTango.READ],
         {
             'format': '%d',
             'description': 'Number of frames lost during the current or last acquisition',
             }],        
       'frame_rate':
        [[PyTango.DevDouble,
          PyTango.SCALAR,
          PyTango.READ],
         {
             'format': '%f',
             'description': 'The last computed frame per second (the value is computed every 100 frames only)',
             }],        
        }

#------------------------------------------------------------------
#    HamamatsuClass Constructor
#------------------------------------------------------------------
    def __init__(self, name):
        PyTango.DeviceClass.__init__(self, name)
        self.set_type(name)

            
#----------------------------------------------------------------------------
#                              Plugins
#----------------------------------------------------------------------------
from Lima  import Hamamatsu as HamamatsuAcq

_HamamatsuCamera = None
_HamamatsuInterface = None

def get_control(camera_number=0, frame_buffer_size=10, **keys) :
    #properties are passed here as string
    global _HamamatsuCamera
    global _HamamatsuInterface
    if _HamamatsuCamera is None:
        print ('\n\nStarting and configuring the Hamamatsu camera ...')
        _HamamatsuCamera = HamamatsuAcq.Camera("useless config_path!!", int(camera_number), int(frame_buffer_size))
        _HamamatsuInterface = HamamatsuAcq.Interface(_HamamatsuCamera)
        print ('\n\nHamamatsu Camera %s: %s is started'%(_HamamatsuCamera.getDetectorType(),_HamamatsuCamera.getDetectorModel()))
    return Core.CtControl(_HamamatsuInterface)

    
def get_tango_specific_class_n_device():
    return HamamatsuClass,Hamamatsu

