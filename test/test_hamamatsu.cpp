//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2023
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
#include "HamamatsuInterface.h"
#include "lima/CtControl.h"
#include "lima/CtAcquisition.h"
#include "lima/CtSaving.h"
#include "lima/CtImage.h"
#ifdef __unix
#include <sys/time.h>
#endif
#include <iostream>

using namespace lima;
using namespace lima::Hamamatsu;
using namespace std;

void hamamatsu_test(double expo, long nframe)
{
	Camera *cam;
	HwInterface *hw;
	CtControl *ct;
	CtAcquisition *acq;
	CtSaving *save;
	CtImage *image;
	CtControl::ImageStatus img_status;
	long frame= -1;

	cout << "HAMAMATSU TEST: starting" << endl;

	cam= new Camera("", 0 , 10);
	hw= new Interface(*cam);
	ct= new CtControl(hw);

	save= ct->saving();
	save->setDirectory("./data");
 	save->setPrefix("test_");
	save->setSuffix(".h5");
	save->setNextNumber(100);
	save->setFormat(CtSaving::HDF5BS);
	save->setSavingMode(CtSaving::AutoFrame);
	save->setFramesPerFile(100);

	Bin bin(2,2);
	image= ct->image();
	image->setBin(bin);

	cout << "HAMAMATSU TEST: " << expo <<" sec / " << nframe << " frames" << endl;

	acq= ct->acquisition();
	acq->setAcqMode(Single);
	acq->setAcqExpoTime(expo);
	acq->setAcqNbFrames(nframe);

	ct->prepareAcq();
   	ct->startAcq();
	cout << "HAMAMATSU TEST: acq started" << endl;

	while (frame < (nframe-1)) {
	    Sleep(0.1);
	    ct->getImageStatus(img_status);
	    if (frame!=img_status.LastImageAcquired) {
		frame= img_status.LastImageAcquired;
	    	cout << "HAMAMATSU TEST: frame nr " << frame << endl;
	    }
	}
	cout << "HAMAMATSU TEST: acq finished" << endl;
	
	ct->stopAcq();
	cout << "HAMAMATSU TEST: acq stopped" << endl;
}

int main(int argc, char *argv[])
{
	double expo;
	long nframe;

	if (argc != 3) {
		expo= 0.5;
		nframe= 5;
	} else {
		expo= atof(argv[1]);
		nframe= atoi(argv[2]);
	}
        try {
                hamamatsu_test(expo, nframe);
        } catch (Exception e) {
	        cerr << "LIMA Exception:" << e.getErrMsg() << endl;
        }

        return 0;
}

