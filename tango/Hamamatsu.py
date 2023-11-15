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
        self.__FastExtTrigger = {'ON':True,
                           'OFF':False}
        self.__Cooler = {'ON': True,
                             'OFF': False}
        self.__ShutterLevel = {'LOW':0,
                                   'HIGH':1}       

        if _HamamatsuInterface.getFanMode() == HamamatsuAcq.FAN_UNSUPPORTED:
            self.__FanMode = {'UNSUPPORTED': HamamatsuAcq.FAN_UNSUPPORTED}
        else:
            self.__FanMode = {'FULL': HamamatsuAcq.FAN_ON_FULL,
                              'LOW': HamamatsuAcq.FAN_ON_LOW,
                              'OFF': HamamatsuAcq.FAN_OFF,
                              }

        if _HamamatsuInterface.getHighCapacity() == HamamatsuAcq.HC_UNSUPPORTED:
            self.__HighCapacity = {'UNSUPPORTED': HamamatsuAcq.HC_UNSUPPORTED}
        else:
            self.__HighCapacity = {'HIGH_CAPACITY': HamamatsuAcq.HIGH_CAPACITY,
                                   'HIGH_SENSITIVITY': HamamatsuAcq.HIGH_SENSITIVITY,
                                   }

        if _HamamatsuInterface.getBaselineClamp() == HamamatsuAcq.BLCLAMP_UNSUPPORTED:
            self.__BaselineClamp = {'UNSUPPORTED': HamamatsuAcq.BLCLAMP_UNSUPPORTED}
        else:
            self.__BaselineClamp = {'ON': HamamatsuAcq.BLCLAMP_ENABLED,
                                    'OFF': HamamatsuAcq.BLCLAMP_DISABLED}

        #Only needed to map attribute and function which does not fit the with naming convention.
        self.__Attribute2FunctionBase = {
            'temperature_sp': 'TemperatureSP',
            #'my_attr1': 'AnOtherFunctionName',
                                       }

        # prepare lists of supported PGain/VerticalShiftSpeed/AdcSpeed
        self.__PGain = {}
        max_ind = _HamamatsuInterface.getPGainMaxIndex()
        for ind in range(max_ind):
            self.__PGain[_HamamatsuInterface.getPGainString(ind)] = ind
        
        self.__VsSpeed = {}
        max_ind = _HamamatsuInterface.getVsSpeedMaxIndex()
        for ind in range(max_ind):
            self.__VsSpeed[_HamamatsuInterface.getVsSpeedString(ind)] = ind

        self.__AdcSpeed = {}
        max_ind = _HamamatsuInterface.getAdcSpeedMaxIndex()
        for ind in range(max_ind):
            self.__AdcSpeed[_HamamatsuInterface.getAdcSpeedPaireString(ind)] = ind
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

        
       # Apply properties if any
        if self.p_gain:
            _HamamatsuInterface.setPGain(self.__PGain[self.p_gain])
            
        if self.vs_speed:
            _HamamatsuInterface.setVsSpeed(self.__VsSpeed[self.vs_speed])
            
        if self.adc_speed:
            _HamamatsuInterface.setAdcSpeed(self.__AdcSpeed[self.adc_speed])

        if self.temperature_sp:            
            _HamamatsuInterface.setTemperatureSP(self.temperature_sp)
            
        if self.cooler:
            _HamamatsuInterface.setCooler(self.__Cooler[self.cooler])
            
        if self.fast_ext_trigger:
            _HamamatsuInterface.setFastExtTrigger(self.__FastExtTrigger[self.fast_ext_trigger])
            
        if self.shutter_level:
            _HamamatsuInterface.setShutterLevel(self.__ShutterLevel[self.shutter_level])


        if 'UNSUPPORTED' in self.__FanMode.keys() and self.fan_mode:
            deb.Error('Cannot set fan_mode property, not supported for this camera model')
        elif self.fan_mode:
                _HamamatsuInterface.setFanMode(self.__FanMode[self.fan_mode])

        if 'UNSUPPORTED' in self.__HighCapacity.keys() and self.high_capacity:
            deb.Error('Cannot set high_capacity property, not supported for this camera model')
        elif self.high_capacity:
                _HamamatsuInterface.setHighCapacity(self.__HighCapacity[self.high_capacity])

        if 'UNSUPPORTED' in self.__BaselineClamp.keys() and self.baseline_clamp:
            deb.Error('Cannot set baseline_clamp propery, not supported for this camera model')
        elif self.baseline_clamp:
            _HamamatsuInterface.setBaselineClamp(self.__BaselineClamp[self.baseline_clamp])

 
        

#==================================================================
#
#    Hamamatsu read/write attribute methods
#
#==================================================================


    def __getattr__(self,name) :
        return AttrHelper.get_attr_4u(self, name, _HamamatsuInterface)


    ## @brief return the timing times, exposure and latency
    #  
    def read_timing(self, attr):
        timing=[]
        timing.append(_HamamatsuCamera.getExpTime())
        timing.append(_HamamatsuCamera.getLatTime())
        
        attr.set_value(timing,2)        
        

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
        'config_path':
        [PyTango.DevString,
         'configuration path directory', []],
        }


    #    Command definitions
    cmd_list = {
        'getAttrStringValueList':
        [[PyTango.DevString, "Attribute name"],
         [PyTango.DevVarStringArray, "Authorized String value list"]]
        }


    #    Attribute definitions
    attr_list = {
        'fast_ext_trigger':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'unit': 'N/A',
             'format': '',
             'description': 'Fast trigger mode, see manual for usage, OFF or ON',
             }],
        'shutter_level':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'unit': 'N/A',
             'format': '',
             'description': 'Shutter output level, see manual for usage LOW or HIGH',
             }],
       'temperature_sp':
        [[PyTango.DevShort,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'unit': 'C',
             'format': '%1d',
             'description': 'in Celsius',
             }],
        'temperature':
        [[PyTango.DevShort,
          PyTango.SCALAR,
          PyTango.READ],
         {
             'unit': 'C',
             'format': '%1d',
             'description': 'in Celsius',
             }],
        'cooler':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'unit': 'N/A',
             'format': '',
             'description': 'Start/stop the cooler, OFF or ON',
             }],
        'cooling_status':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ],
         {
             'label':'Cooling status',
             'unit': 'N/A',
             'format': '%1d',
             'description': 'return status of the cooling system, tell if the setpoint is reached',
             }],
        'timing':
        [[PyTango.DevFloat,
          PyTango.SPECTRUM,
          PyTango.READ,2],
        {
             'unit': 'second',
             'format': '%f',
             'description': '[0]: exposure, [1]: latency',
             }],
        'p_gain':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'unit': 'N/A',
             'format': '%s',
             'description': 'Premplifier Gain which can be apply to the readout, from X1-XN, check the camera documentation for the valid range',
             }],
        'vs_speed':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'unit': 'N/A',
             'format': '',
             'description': 'Vertical shift speed,  in us/pixel, check the camera documentation for the valid range',
             }],
        'adc_speed':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'unit': 'N/A',
             'format': '',
             'description': 'ADC and Horizontal shift speed, in ADCchannel/Freq.Mhz, check the documentatio for more help',
             }],
        'high_capacity':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'unit': 'N/A',
             'format': '',
             'description': 'HIGH_CAPACITY or HIGH_SENSITIVITY',
             }],
        'fan_mode':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'unit': 'N/A',
             'format': '',
             'description': 'FAN_OFF or FAN_ON_FULL, or FAN_ON_LOW',
             }],
        'baseline_clamp':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE],
         {
             'unit': 'N/A',
             'format': '',
             'description': 'ON or OFF',
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

def get_control(config_path='/usr/local/etc/andor', serial_number=0, **keys) :
    #properties are passed here as string
    global _HamamatsuCamera
    global _HamamatsuInterface
    if _HamamatsuCamera is None:
        print ('\n\nStarting and configuring the Hamamatsu camera ...')
        _HamamatsuCamera = HamamatsuAcq.Camera(config_path, int(serial_number))
        _HamamatsuInterface = HamamatsuAcq.Interface(_HamamatsuCamera)
        print ('\n\nHamamatsu Camera %s: %s is started'%(_HamamatsuCamera.getDetectorType(),_HamamatsuCamera.getDetectorModel()))
    return Core.CtControl(_HamamatsuInterface)

    
def get_tango_specific_class_n_device():
    return HamamatsuClass,Hamamatsu

