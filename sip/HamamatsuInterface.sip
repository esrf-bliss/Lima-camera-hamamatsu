//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2014
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################

namespace Hamamatsu
{
/*******************************************************************
* \class Interface
* \brief Hamamatsu hardware interface
*******************************************************************/
class Interface : HwInterface
{

%TypeHeaderCode
#include <HamamatsuInterface.h>
%End

public:
	Interface(Hamamatsu::Camera& cam /KeepReference/);
	virtual ~Interface();

	//- From HwInterface
	virtual void getCapList(std::vector<HwCap> &cap_list /Out/) const;
	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status /Out/);
	virtual int getNbHwAcquiredFrames();

	// get the camera object to access it directly from client
	Hamamatsu::Camera& getCamera();
private:
        Interface(const Hamamatsu::Interface&);
};

}; // end Hamamatsu namespace
