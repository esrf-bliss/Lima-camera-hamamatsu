//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2017
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

class Camera
{

%TypeHeaderCode
#include <HamamatsuCamera.h>
%End

public:

	enum Status { Ready, Exposure, Readout, Latency, Fault };

	Camera(const std::string& config_path,int camera_number=0);
	~Camera();

	void startAcq();
	void stopAcq();
	void prepareAcq();

	// -- detector info object
	void getImageType(ImageType& type /Out/);
	void setImageType(ImageType type);

	// detector info object
	void getDetectorType(std::string& type /Out/);
	void getDetectorModel(std::string& model /Out/);
	void getDetectorImageSize(Size& size /Out/);
	void getDetectorMaxImageSize(Size& size /Out/);

	// synch control object
	bool checkTrigMode(TrigMode trig_mode);
	void setTrigMode(TrigMode mode);
	void getTrigMode(TrigMode& mode /Out/);	

	void setExpTime(double exp_time);
	void getExpTime(double& exp_time /Out/);

	void setLatTime(double lat_time);
	void getLatTime(double& lat_time /Out/);

	void getExposureTimeRange(double& min_expo /Out/, double& max_expo /Out/) const;
	void getLatTimeRange(double& min_lat /Out/, double& max_lat /Out/) const;

	void setNbFrames(int nb_frames);
	void getNbFrames(int& nb_frames /Out/);
	void getNbHwAcquiredFrames(int &nb_acq_frames /Out/);

	void checkRoi(const Roi& set_roi, Roi& hw_roi);
	void setRoi(const Roi& set_roi);
	void getRoi(Roi& hw_roi /Out/);
	
	void checkBin(Bin&);
	void setBin(const Bin&);
	void getBin(Bin&  /Out/);
	bool isBinningAvailable();
	
	void getPixelSize(double& sizex, double& sizey);
	
	Status getStatus();
	
	void reset();

	// Hamamatsu specific
	void initialiseController();
	short getReadoutSpeed();
	void setReadoutSpeed(const short readoutSpeed);
	void getLostFrames(unsigned long& lostFrames /Out/);
	void getFPS(double& fps /Out/);
	double getSensorTemperature();
private:
        Camera(const Hamamatsu::Camera&);
};

}; // end Hamamatsu namespace
